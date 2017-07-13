// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "WNDoor.h"
#include "WNGameMode.h"

// Sets default values
AWNDoor::AWNDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWNDoor::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	if (GMBase != nullptr)
	{
		AWhiteNoiseGameMode* GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
		if (GMWNBase != nullptr)
		{
			GMWNBase->Register(this, false);
		}
	}

	this->Close();
}

// Called every frame
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
