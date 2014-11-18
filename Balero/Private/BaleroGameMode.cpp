// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Balero.h"
#include "BaleroGameMode.h"
#include "BaleroPlayerController.h"
#include "BaleroCharacter.h"

ABaleroGameMode::ABaleroGameMode(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	// use our custom PlayerController class
	PlayerControllerClass = ABaleroPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/MyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}