// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "EventExampleProject.h"
#include "EventExampleProjectGameMode.h"
#include "EventExampleProjectPlayerController.h"
#include "EventExampleProjectCharacter.h"

AEventExampleProjectGameMode::AEventExampleProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AEventExampleProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/SpectatorCam"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}