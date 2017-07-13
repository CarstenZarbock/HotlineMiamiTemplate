// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "GameFramework/Actor.h"
#include "WNDoor.generated.h"

UCLASS()
class WHITENOISE_API AWNDoor : public AActor
{
	GENERATED_BODY()
protected:
	bool bIsOpen;

public:	
	// Sets default values for this actor's properties
	AWNDoor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void Open(FVector worldDirection);
	virtual void Close();
};
