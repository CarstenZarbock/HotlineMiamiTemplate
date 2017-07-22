// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "GameFramework/Actor.h"
#include "WNDoor.generated.h"

UCLASS()
class WHITENOISE_API AWNDoor : public AActor
{
	GENERATED_BODY()
private:
	/** Register the actor on Game Mode / StageHandler */
	bool RegisterOnGameMode();

protected:

	/** */
	bool bIsOpen;

public:	
	/** */
	AWNDoor();

	/** */
	virtual void BeginPlay() override;
	
	/** */
	virtual void Tick( float DeltaSeconds ) override;

	/** */
	virtual void Open(FVector worldDirection);

	/** */
	virtual void Close();
};
