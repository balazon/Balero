// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Balero.h"
#include "BaleroPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "math.h"

#include "BaleroCharacter.h"
#include "GameFramework/HUD.h"
#include "AIControllerBase.h"

#include "BalaLib.h"
#include "Formation.h"
#include "UnitGroup.h"



#define SELECTION_INITSIZE 10


ABaleroPlayerController::ABaleroPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	SelectedUnits.Init(SELECTION_INITSIZE);
	SelectedUnits.Empty(SELECTION_INITSIZE);

	Delta = 0.f;



}

void ABaleroPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}

	Delta = DeltaTime;
}


void ABaleroPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ABaleroPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ABaleroPlayerController::OnSetDestinationReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ABallertsPlayerController::TouchPressed);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ABallertsPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("Click", IE_Pressed, this, &ABaleroPlayerController::OnClickPressed);
	InputComponent->BindAction("Click", IE_Released, this, &ABaleroPlayerController::OnClickReleased);

	InputComponent->BindAction("IncreaseSelection", IE_Pressed, this, &ABaleroPlayerController::OnIncreaseSelectionPressed);
	InputComponent->BindAction("IncreaseSelection", IE_Released, this, &ABaleroPlayerController::OnIncreaseSelectionReleased);

	InputComponent->BindAction("Zoom", IE_Pressed, this, &ABaleroPlayerController::OnZoomStart);

	InputComponent->BindAction("Zoom", IE_Released, this, &ABaleroPlayerController::OnZoomProcessing);

	//InputComponent->BindAction("MoveForward", IE_Pressed, this, &ABallertsPlayerController::OnPanVerticalStart);

	//InputComponent->BindAction("MoveRight", IE_Pressed, this, &ABallertsPlayerController::OnPanHorizontalStart);

	InputComponent->BindAxis("MoveRight", this, &ABaleroPlayerController::OnPanVerticalStart);

	InputComponent->BindAxis("MoveForward", this, &ABaleroPlayerController::OnPanHorizontalStart);

	InputComponent->BindAction("RightClick", IE_Pressed, this, &ABaleroPlayerController::OnRightClickPressed);

	InputComponent->BindAction("Test", IE_Pressed, this, &ABaleroPlayerController::OnTestButtonPressed);



}

void ABaleroPlayerController::BeginPlay()
{
	Super::BeginPlay();

	AHUD* Hud = GetHUD();
	bEnableClickEvents = true;
	bShowMouseCursor = true;
	Hud->EnableInput(this);

	UUnitGroup::SetWorld(GetWorld());



}

void ABaleroPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ABaleroPlayerController::TouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{

}

void ABaleroPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ABaleroPlayerController::MoveToActor(AActor* TargetActor)
{
	for (ABaleroCharacter* MyChar : SelectedUnits)
	{
		if (MyChar)
		{
			AAIControllerBase* controller = Cast<AAIControllerBase>(MyChar->GetController());
			controller->SetTargetActor(TargetActor);
		}
	}
}

void ABaleroPlayerController::SetNewMoveDestination(const FVector DestLocation)
{

	UUnitGroup::Move(FVector2D(DestLocation.X, DestLocation.Y), SelectedUnits);

}

void ABaleroPlayerController::MoveToFormation()
{
	UUnitGroup::MoveToFormation(SelectedUnits, EShapeEnum::SE_ONE_TRIANGLE);

}

void ABaleroPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	//bMoveToMouseCursor = true;
}

void ABaleroPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	//bMoveToMouseCursor = false;
}

void ABaleroPlayerController::DeselectAll()
{
	UE_LOG(LogTemp, Warning, TEXT("DESELECT ALL"));
	for (ABaleroCharacter* Unit : SelectedUnits)
	{
		if (Unit)
		{
			Unit->SelectedEffect->SetVisibility(false);
		}
	}
	SelectedUnits.Init(SELECTION_INITSIZE);
	SelectedUnits.Empty(SELECTION_INITSIZE);

}

void ABaleroPlayerController::OnClickPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("clickpressed"));
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	FVector2D MouseLocation;
	GetMousePosition(MouseLocation.X, MouseLocation.Y);

	int32 ViewportX;
	int32 ViewportY;
	GetViewportSize(ViewportX, ViewportY);

	if (MouseLocation.X > ViewportX * .9f)
	{
		return;
	}


	if (Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("clickedsomething"));


		APawn* pawn = Cast<APawn>(Hit.GetActor());
		if (pawn != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("clicked pawn"));

			ABaleroCharacter* Unit = Cast<ABaleroCharacter>(pawn);
			if (Unit != NULL)
			{
				//deselect the clicked unit
				if (SelectedUnits.Contains(Unit))
				{
					RemoveUnitFromSelection(Unit);
				}
				//add unit to selection
				else
				{
					AddUnitToSelection(Unit);
				}

			}
		}
		else
		{
			//give move command to move
			UE_LOG(LogTemp, Warning, TEXT("MOVE COMMAND_A"));
			for (ABaleroCharacter* Unit : SelectedUnits)
			{
				AAIControllerBase* AIControl = Cast<AAIControllerBase>(Unit->GetController());
				FVector loc = Unit->GetActorLocation();
				UE_LOG(LogTemp, Warning, TEXT("Unit ai: %s , pos: %.1f %.1f %.1f"), *(AActor::GetDebugName(AIControl)), loc.X, loc.Y, loc.Z);

			}
			MoveToMouseCursor();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MOVE COMMAND_B"));
		MoveToMouseCursor();
	}
}

void ABaleroPlayerController::OnClickReleased()
{

}

void ABaleroPlayerController::OnIncreaseSelectionPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("shiftclickpressed"));

	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		APawn* pawn = Cast<APawn>(Hit.Actor.Get());
		if (pawn != NULL)
		{
			ABaleroCharacter* Unit = Cast<ABaleroCharacter>(pawn);
			if (Unit != NULL)
			{
				SelectedUnits.Add(Unit);
				Unit->SelectedEffect->SetVisibility(true);
			}
		}
	}
}

void ABaleroPlayerController::OnIncreaseSelectionReleased()
{

}


void ABaleroPlayerController::OnZoomStart()
{

}

void ABaleroPlayerController::OnZoomProcessing()
{

}

void ABaleroPlayerController::OnPanVerticalStart(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("RIGHT -- LEFT"));

	GetPawn()->AddMovementInput(FVector(0, 10.f, 0), value);
	if (true)
	{
		GetPawn()->AddActorLocalOffset(FVector(0, 500.f * value * Delta, 0));
		//UE_LOG(LogTemp, Warning, TEXT("RIGHT -- LEFT"));

	}
	//SetActorLocation()
}

void ABaleroPlayerController::OnPanVerticalProcessing(float value)
{

}


void ABaleroPlayerController::OnPanHorizontalStart(float value)
{
	GetPawn()->AddMovementInput(FVector(10.f, 0, 0), value);
	//FMath::Abs(value) > .1f
	if (true)
	{
		GetPawn()->AddActorLocalOffset(FVector(500.f * value * Delta, 0, 0));
		//UE_LOG(LogTemp, Warning, TEXT("RIGHT -- LEFT"));

	}
}

void ABaleroPlayerController::OnPanHorizontalProcessing(float value)
{

}



void ABaleroPlayerController::OnRightClickPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("rightclick"));
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	FVector2D MouseLocation;
	GetMousePosition(MouseLocation.X, MouseLocation.Y);

	int32 ViewportX;
	int32 ViewportY;
	GetViewportSize(ViewportX, ViewportY);

	if (MouseLocation.X > ViewportX * .9f)
	{
		return;
	}


	if (Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("rightclickedsomething"));


		APawn* pawn = Cast<APawn>(Hit.GetActor());
		if (pawn != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("clicked pawn"));

			ABaleroCharacter* Unit = Cast<ABaleroCharacter>(pawn);
			if (Unit != NULL)
			{
				MoveToActor(Unit);
			}
		}
		else
		{
			MoveToMouseCursor();
		}
	}


}


void ABaleroPlayerController::AddUnitToSelection(ABaleroCharacter* Unit)
{
	SelectedUnits.Add(Unit);
	Unit->SelectedEffect->SetVisibility(true);
}

void ABaleroPlayerController::RemoveUnitFromSelection(ABaleroCharacter* Unit)
{
	SelectedUnits.Remove(Unit);
	Unit->SelectedEffect->SetVisibility(false);
}

void ABaleroPlayerController::OnTestButtonPressed()
{
	//TArray<float> Weights;

	//
	//Weights.Init(10000);
	//for (int i = 0; i < 10000; i++) Weights[i] = FMath::FRandRange(1.f, 10.f);

	//TArray<int32> res;
	//UBalaLib::Assignment(Weights, 100, res);

	////assignment has a while(true), but if the algorithm was implemented well, it shouldnt be a problem
	//UE_LOG(LogTemp, Warning, TEXT("Assignment OK"));

	if (SelectedUnits.Num() > 0)
	{
		AAIControllerBase* Controller = Cast<AAIControllerBase>(SelectedUnits[0]->GetController());

		Controller->MyTestFunc();
	}



}