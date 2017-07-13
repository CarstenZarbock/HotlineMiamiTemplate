// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "NPC_Humanoid.h"
#include "NPC_Humanoid_Guard.generated.h"

/**
 * 
 */
UCLASS()
class WHITENOISE_API Anpc_guard : public AHumanoid
{
	GENERATED_BODY()
private:
	void HandleMovementTypes();
	void Handle_Idle();
	void Handle_RandomPatrol();
	void Handle_FixedPatrol();
	bool CanMoveInDirection(FVector WorldDirection, float fCheckDistance);
	float testnext = 0;
public:
	Anpc_guard();
	virtual void HandleAI() override;	
};
