// Fill out your copyright notice in the Description page of Project Settings.

#include "Balero.h"
#include "UnitGroup.h"

#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"

#include "BaleroCharacter.h"
#include "AIControllerBase.h"

#include "BalaLib.h"
#include "Formation.h"

#include "Engine.h"


UUnitGroup::UUnitGroup(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Formation = NewObject<class UFormation>();
	CurrentShape = EShapeEnum::SE_NONE;
}

UWorld* UUnitGroup::World = NULL;


void UUnitGroup::MoveToFormationTriangle_Implementation()
{
	//calculate the triangle's shape
	FVector2D Center = FVector2D(0.f, 0.f);
	int count = 0;
	float sumCapDiameter = 0.f;
	for (ABaleroCharacter* MyChar : Units)
	{
		if (MyChar)
		{
			FVector loc = MyChar->GetActorLocation();
			UCapsuleComponent* cap = Cast<UCapsuleComponent>(MyChar->GetComponentByClass(UCapsuleComponent::StaticClass()));
			sumCapDiameter += cap->GetScaledCapsuleRadius() * 2.f;
			Center += FVector2D(loc.X, loc.Y);
			count++;
		}
	}

	Center /= count;
	UE_LOG(LogTemp, Warning, TEXT("Center: %.2f %.2f"), Center.X, Center.Y);



	float triEdge = sumCapDiameter / 3.f * 4.f;

	Formation->ClearShapes();
	Formation->AddTriangle(Center, triEdge, Units.Num());

	Formation->AssignAllUnits(Units);
}




void UUnitGroup::RecalculateMovement()
{
	//TODO make this code clean, currently it is roughly just copied from the old setdestination func


	float minDistance = 10000.f;
	ABaleroCharacter* ClosestUnit = NULL;
	UNavigationSystem* const NavSys = World->GetNavigationSystem();

	//UE_LOG(LogTemp, Warning, TEXT("%.3f"), DestLocation.Z);
	bool PathFound = false;
	for (ABaleroCharacter* Unit : Units)
	{
		if (Unit)
		{
			// We need to issue move command only if far enough in order for walk animation to play correctly
			if (NavSys)
			{
				//calculate nav path length for determining the closest pawn which will be the leader
				UNavigationPath * MyPath = NavSys->FindPathToLocationSynchronously(GetWorld(), Unit->GetActorLocation(), FVector(CurrentTarget.X, CurrentTarget.Y, 120.f), Unit, 0);
				float dist = MyPath->GetPathLength();
				//UE_LOG(LogTemp, Warning, TEXT("pathdistance %.2f"), dist);

				if (dist > 0.f && dist < minDistance) {
					minDistance = dist;
					ClosestUnit = Unit;
					PathFound = true;
				}
				//Path->exec
				//NavSys->FindPathToLocationSynchronously(GetWorld(), //NavSys->GetPathLength()
			}
		}
	}
	//if navigation couldnt find a path, then check air distance
	if (Units.Num() > 0 && !PathFound)
	{

		for (ABaleroCharacter* Unit : Units)
		{
			float dist = FVector2D::Distance(CurrentTarget, Unit->GetActorLocation2D());
			if (dist < minDistance) {
				minDistance = dist;
				ClosestUnit = Unit;
			}

		}
		PathFound = true;
	}
	if (PathFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("leader is: %s"), *(ClosestUnit->GetHumanReadableName()));

		//the leader will go to destination, others should follow him
		//NavSys->SimpleMoveToLocation(ClosestChar->GetController(), DestLocation);
		Leader = ClosestUnit;

		FVector2D LeaderLoc = Leader->GetActorLocation2D();
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Leader is: %.1f %.1f"), LeaderLoc.X, LeaderLoc.Y));

		Formation->BindAllShapesToLeader(Leader);

		AAIControllerBase* Controller = Cast<AAIControllerBase>(Leader->GetController());
		Controller->LeaderTarget = CurrentTarget;
	}
}

void UUnitGroup::Move(FVector2D Target, const TArray<ABaleroCharacter*> SelectedUnits)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("UUnitGroup::Move"));
	UUnitGroup* UnitGroup = NULL;
	TArray<UUnitGroup*> OldGroups;
	OldGroups.Empty();
	if (SelectedUnits.Num() > 0)
	{
		UnitGroup = SelectedUnits[0]->UnitGroup;
	}
	else
	{
		return;
	}
	for (int i = 0; i < SelectedUnits.Num(); i++)
	{
		UUnitGroup* OldGroup = SelectedUnits[i]->UnitGroup;
		if (UnitGroup != OldGroup && OldGroup != NULL)
		{
			OldGroups.Add(SelectedUnits[i]->UnitGroup);
		}
	}
	for (int i = 0; i < OldGroups.Num(); i++)
	{
		UUnitGroup* OldGroup = OldGroups[i];
		for (int j = 0; j < SelectedUnits.Num(); j++)
		{
			OldGroup->RemoveUnit(SelectedUnits[j]);
		}
		OldGroup->RecalculateMovement();
	}
	bool AlreadyInUnitGroup = (UnitGroup != NULL) && (OldGroups.Num() == 0);
	if (AlreadyInUnitGroup)
	{
		if (UnitGroup->Num() > SelectedUnits.Num())
		{
			//oldgroup contains all the selected units and more
			//now it will contain only the "more"
			UUnitGroup* OldGroup = UnitGroup;
			for (int i = 0; i < SelectedUnits.Num(); i++)
			{
				OldGroup->RemoveUnit(SelectedUnits[i]);

			}


			UnitGroup = NewObject<class UUnitGroup>();
			UnitGroup->SetUnits(SelectedUnits);
			UnitGroup->CurrentTarget = Target;
			UnitGroup->RecalculateMovement();

			if (SelectedUnits.Contains(UnitGroup->Leader))
			{
				OldGroup->RecalculateMovement();
			}
		}
		else
		{
			UnitGroup->SetUnits(SelectedUnits);
			UnitGroup->CurrentTarget = Target;
			UnitGroup->RecalculateMovement();
		}
	}
	if (!AlreadyInUnitGroup)
	{
		UnitGroup = NewObject<class UUnitGroup>();
		UnitGroup->SetUnits(SelectedUnits);
		UnitGroup->CurrentTarget = Target;
		UnitGroup->RecalculateMovement();
	}
}

void UUnitGroup::MoveToFormation(const TArray<ABaleroCharacter*> SelectedUnits, const TEnumAsByte<EShapeEnum::Type>& ShapeType)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("UUnitGroup::MoveToFormation"));
	UUnitGroup* UnitGroup = NULL;
	TArray<UUnitGroup*> OldGroups;
	OldGroups.Empty();
	if (SelectedUnits.Num() > 0)
	{
		UnitGroup = SelectedUnits[0]->UnitGroup;
	}
	else
	{
		return;
	}
	for (int i = 0; i < SelectedUnits.Num(); i++)
	{
		UUnitGroup* OldGroup = SelectedUnits[i]->UnitGroup;
		if (UnitGroup != OldGroup && OldGroup != NULL)
		{
			OldGroups.Add(SelectedUnits[i]->UnitGroup);
		}
	}
	//removing selected units from unitgroups
	for (int i = 0; i < OldGroups.Num(); i++)
	{
		UUnitGroup* OldGroup = OldGroups[i];
		for (int j = 0; j < SelectedUnits.Num(); j++)
		{
			OldGroup->RemoveUnit(SelectedUnits[j]);
		}
		OldGroup->RecalculateMovement();
	}
	bool AlreadyInUnitGroup = (UnitGroup != NULL) && (OldGroups.Num() == 0);
	if (AlreadyInUnitGroup)
	{
		if (UnitGroup->Num() > SelectedUnits.Num())
		{
			//oldgroup contains all the selected units and more
			//now it will contain only the "more"
			UUnitGroup* OldGroup = UnitGroup;
			for (int i = 0; i < SelectedUnits.Num(); i++)
			{
				OldGroup->RemoveUnit(SelectedUnits[i]);

			}


			UnitGroup = NewObject<class UUnitGroup>();
			UnitGroup->SetUnits(SelectedUnits);
			UnitGroup->MoveToFormationPreset(ShapeType);


			if (SelectedUnits.Contains(UnitGroup->Leader))
			{
				OldGroup->RecalculateMovement();
			}
		}
		else
		{
			UnitGroup->SetUnits(SelectedUnits);
			UnitGroup->MoveToFormationPreset(ShapeType);
		}
	}
	if (!AlreadyInUnitGroup)
	{
		UnitGroup = NewObject<class UUnitGroup>();
		UnitGroup->SetUnits(SelectedUnits);
		UnitGroup->MoveToFormationPreset(ShapeType);
	}


}

void UUnitGroup::MoveToFormationPreset(const TEnumAsByte<EShapeEnum::Type>& ShapeType)
{
	CurrentShape = ShapeType;
	switch (ShapeType)
	{
	case EShapeEnum::SE_ONE_TRIANGLE:
		MoveToFormationTriangle();
		break;
	case EShapeEnum::SE_SQUARE:
		break;
	case EShapeEnum::SE_NONE:
		break;
	}

}


void UUnitGroup::SetWorld(UWorld* _World)
{
	World = _World;
}

void UUnitGroup::SetUnits(TArray<ABaleroCharacter*> TheUnits)
{
	Units = TheUnits;

	for (int i = 0; i < Units.Num(); i++)
	{
		Units[i]->UnitGroup = this;
	}
}


void UUnitGroup::AddUnit(ABaleroCharacter* Unit)
{
	Units.Add(Unit);
}

void UUnitGroup::RemoveUnit(ABaleroCharacter* Unit)
{
	//maintain order and flatten array
	Units.Remove(Unit);
	TArray<ABaleroCharacter*> UnitCopy = Units;
	Units.Empty();
	for (int i = 0; i < UnitCopy.Num(); i++)
	{
		ABaleroCharacter* AUnit = UnitCopy[i];
		if (AUnit)
		{
			Units.Add(AUnit);
		}
	}
}

int32 UUnitGroup::Num()
{
	return Units.Num();
}