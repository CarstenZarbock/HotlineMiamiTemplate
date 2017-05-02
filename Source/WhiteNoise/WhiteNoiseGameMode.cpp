// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "WhiteNoise.h"
#include "WhiteNoiseGameMode.h"
#include "WhiteNoisePlayerController.h"
#include "WhiteNoiseCharacter.h"

AWhiteNoiseGameMode::AWhiteNoiseGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AWhiteNoisePlayerController::StaticClass();
}