
#include "Balero.h"
#include "AIControllerBase.h"

#include "FormationShape.h"
#include "Engine.h"


AAIControllerBase::AAIControllerBase(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}


void AAIControllerBase::SetTargetActor_Implementation(AActor* Actor)
{

}

void AAIControllerBase::SetTargetLocation_Implementation(const FVector& Location)
{

}

void AAIControllerBase::SetTargetLeader_Implementation(AActor* Actor, const FVector& RelVectorFromLeader)
{

}

void AAIControllerBase::SetTargetLocationAsLeader_Implementation(const FVector& Location, const TArray<ABaleroCharacter*>& Followers)
{

}


void AAIControllerBase::SetShape_Implementation(UFormationShape* Shape)
{
	CurrentShape = Shape;
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, TEXT("Controller super setSHape OK"));
	UpdateBehavior();
}

//void AAIControllerBase::UpdateBehavior_Implementation()
//{
//
//}


void AAIControllerBase::MyTestFunc_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("aibase testfunc cpp"));
}