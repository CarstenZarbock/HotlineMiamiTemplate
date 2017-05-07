// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "AIs/Humanoid.h"
#include "npc_guard.generated.h"

/**
 * 
 */
UCLASS()
class WHITENOISE_API Anpc_guard : public AHumanoid
{
	GENERATED_BODY()
	
public:
	Anpc_guard();
	virtual void HandleAI() override;	
};
