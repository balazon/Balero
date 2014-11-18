// Fill out your copyright notice in the Description page of Project Settings.

#include "Balero.h"
#include "Formation.h"
#include "BalaLib.h"


UFormation::UFormation(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Shapes.Empty();
}


void UFormation::ClearShapes()
{
	Shapes.Empty();
}

void UFormation::AddTriangle(const FVector2D& Center, float side, const int32 N)
{

	Shapes.Add(UFormationShape::CreateTriangle(Center, side, N));
}



void UFormation::BindAllShapesToLeader(ABaleroCharacter* TheLeader)
{
	for (int i = 0; i < Shapes.Num(); i++)
	{
		Shapes[i]->BindToLeader(TheLeader);
	}
}

void UFormation::AssignAllUnits(const TArray<ABaleroCharacter*>& Units)
{
	UFormationShape::AssignShapesToUnits(Shapes, Units);
}
