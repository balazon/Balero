// Fill out your copyright notice in the Description page of Project Settings.



#pragma once

#include "BaleroCharacter.h"

#include "AIController.h"
#include "AIControllerBase.generated.h"


class UFormationShape;

/**
*
*/
UCLASS()
class BALERO_API AAIControllerBase : public AAIController
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintNativeEvent, Category = Commands)
	void SetShape(UFormationShape* Shape);

	UFUNCTION(BlueprintImplementableEvent, Category = Commands)
	void UpdateBehavior();

	UFUNCTION(BlueprintNativeEvent, Category = Commands)
	void SetTargetLocation(const FVector& Location);

	UFUNCTION(BlueprintNativeEvent, Category = Commands)
	void SetTargetActor(AActor* Actor);

	UFUNCTION(BlueprintNativeEvent, Category = Commands)
	void SetTargetLeader(AActor* Actor, const FVector& RelVectorFromLeader);


	UFUNCTION(BlueprintNativeEvent, Category = Commands)
	void SetTargetLocationAsLeader(const FVector& Location, const TArray<ABaleroCharacter*>& Followers);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Formation)
	UFormationShape* CurrentShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Formation)
	FVector2D LeaderTarget;

	UFUNCTION(BlueprintNativeEvent, Category = Test)
	void MyTestFunc();
};
