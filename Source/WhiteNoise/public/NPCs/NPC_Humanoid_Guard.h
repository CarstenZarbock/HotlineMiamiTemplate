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
	/** AI moving based on IDLE / RANDOMPATROL / FIXEDPATROL */
	void HandleMovementTypes();

	/** Currently placeholder / dummy */
	void Handle_Idle();

	/**
	* "Oldschool" patroling without fixed waypoints
	* finds a path with raycasts, "random" walking along the walls
	* pathfinding based on enum movement priority
	*/
	void Handle_RandomPatrol();

	/** Pathfinding on a fixed path based on waypoints */
	void Handle_FixedPatrol();

	/** */
	bool CanMoveInDirection(FVector WorldDirection, float fCheckDistance);

	/**
	* Temp solution for AI, delay for path finding raycast
	* without the delay, AI does rotate inf.
	* AI needs to be redone though.
	*/
	float testnext = 0;

public:
	/** */
	Anpc_guard();

	/** */
	virtual void HandleAI() override;	
};
