// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "WhiteNoiseGameMode.h"
#include "WhiteNoisePlayerController.h"
#include "WhiteNoiseCharacter.h"

AWhiteNoiseGameMode::AWhiteNoiseGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AWhiteNoisePlayerController::StaticClass();
}