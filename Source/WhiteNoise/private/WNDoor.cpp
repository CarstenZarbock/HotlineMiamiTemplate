// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "WNDoor.h"
#include "WNGameMode.h"

AWNDoor::AWNDoor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AWNDoor::BeginPlay()
{
	Super::BeginPlay();

	this->RegisterOnGameMode();

	this->Close();
}

bool AWNDoor::RegisterOnGameMode()
{
	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	if (GMBase != nullptr)
	{
		AWhiteNoiseGameMode* GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
		if (GMWNBase != nullptr)
		{
			return GMWNBase->Register(this, false);
		}
	}

	return false;
}

void AWNDoor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AWNDoor::Open(FVector worldDirection)
{

}

void AWNDoor::Close()
{

}
