// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "BaleroCharacter.h"
#include "Formation.h"

#include "Object.h"
#include "UnitGroup.generated.h"


UENUM(BlueprintType)
namespace EShapeEnum
{
	enum Type
	{
		SE_ONE_TRIANGLE,
		SE_SQUARE,
		SE_NONE
	};
}

//USTRUCT(BlueprintType)
//struct FUnitStruct
//{
//	GENERATED_USTRUCT_BODY()
//
//	UPROPERTY()
//	FVector2D AssignedPoint;
//
//	FUnitStruct()
//	{
//		AssignedPoint = FVector2D::ZeroVector;
//	}
//};

/**
*
*/
UCLASS()
class BALERO_API UUnitGroup : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitGroup)
	TArray<ABaleroCharacter*> Units;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitGroup)
	ABaleroCharacter* Leader;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitGroup)
	FVector2D CurrentTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnitGroup)
	TEnumAsByte<EShapeEnum::Type> CurrentShape;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitGroup)
	UFormation* Formation;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitGroup)
	TMap<ABallertsCharacter*, FUnitStruct> UnitData;*/


	UFUNCTION(BlueprintCallable, Category = MoveCommand)
	static void Move(FVector2D Target, const TArray<ABaleroCharacter*> SelectedUnits);

	UFUNCTION(BlueprintCallable, Category = FormationCommand)
	static void MoveToFormation(const TArray<ABaleroCharacter*> SelectedUnits, const TEnumAsByte<EShapeEnum::Type>& ShapeType);

	UFUNCTION(BlueprintCallable, Category = UnitGroup)
	void MoveToFormationPreset(const TEnumAsByte<EShapeEnum::Type>& ShapeType);



	UFUNCTION(BlueprintCallable, Category = UnitGroup)
	void SetUnits(TArray<ABaleroCharacter*> TheUnits);

	UFUNCTION(BlueprintCallable, Category = UnitGroup)
	void AddUnit(ABaleroCharacter* Unit);

	UFUNCTION(BlueprintCallable, Category = UnitGroup)
	void RemoveUnit(ABaleroCharacter* Unit);

	UFUNCTION(BlueprintCallable, Category = UnitGroup)
		int32 Num();

	/*UFUNCTION(BlueprintNativeEvent, Category = UnitGroup)
	void SetDestination(const FVector& Destination);*/

	UFUNCTION(BlueprintCallable, Category = UnitGroup)
		static void SetWorld(UWorld* _World);







protected:
	static UWorld* World;

	UFUNCTION(BlueprintNativeEvent, Category = UnitGroup)
	void MoveToFormationTriangle();

	//calculating leader and giving move commands
	UFUNCTION(BlueprintCallable, Category = UnitGroup)
	void RecalculateMovement();



};
