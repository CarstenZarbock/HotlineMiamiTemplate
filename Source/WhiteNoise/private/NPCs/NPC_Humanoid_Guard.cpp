// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "NPC_Humanoid_Guard.h"
#include "WNDoor.h"

/* basic enemy npc, can carry weapon, can patrol around */
Anpc_guard::Anpc_guard()
{

}

void Anpc_guard::HandleAI()
{
	if (this->bIsAlive)
	{
		if (this->bIsCrawling)
		{
			this->HandleCrawl();
		}
		else
		{
			if (this->bHasSeenPlayer)
			{
				/* Player has been seen*/
				if (this->PlayerInSight())
				{
					//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, "[AI] PLAYER IN SIGHT");
				}
				else
				{
					/* Player is not insight anymore */
					this->bHasSeenPlayer = false;
				}
			}
			else
			{
				if (this->PlayerInSight())
				{
					/* Set Player is in Sight, Handle next Tick*/
					this->bHasSeenPlayer = true;
					return;
				}
				else
				{
					/* Default Action, Path Walking, etc*/
					this->HandleMovementTypes();
				}
			}
		}
	}
}

void Anpc_guard::HandleMovementTypes()
{
	switch (this->EEnemyType)
	{
	case EEnemyMovement::EM_IDLE:
		this->Handle_Idle();
		break;
	case EEnemyMovement::EM_RANDOMPATROL:
		this->Handle_RandomPatrol();
		break;
	case EEnemyMovement::EM_FIXEDPATROL:
		this->Handle_FixedPatrol();
		break;
	default:
		break;
	}
}

void Anpc_guard::Handle_Idle()
{

}

bool Anpc_guard::CanMoveInDirection(FVector WorldDirection, float fCheckDistance)
{
	FVector InitialPosition = this->GetActorLocation();
	FVector InitialHitPosition = InitialPosition + (WorldDirection * fCheckDistance);

	FHitResult Hit = TraceLine(InitialHitPosition, InitialHitPosition, false);

	if (Hit.bBlockingHit)
	{
		AWNDoor* Door = Cast<AWNDoor>(Hit.GetActor());
		if (Door)
		{
			return true;
		}
	}

	FVector Hit2StartPosition = InitialPosition + (WorldDirection.RotateAngleAxis(90.0f, FVector(0, 0, 1)) * 75.0f);
	FVector Hit2EndPosition = InitialHitPosition + (WorldDirection.RotateAngleAxis(90.0f, FVector(0, 0, 1)) * 75.0f);

	FHitResult Hit2 = TraceLine(Hit2StartPosition, Hit2EndPosition, false);
	if (Hit2.bBlockingHit)
	{
		AWNDoor* Door = Cast<AWNDoor>(Hit2.GetActor());
		if (Door)
		{
			return true;
		}
	}

	FVector Hit3StartPosition = InitialPosition + ((WorldDirection.RotateAngleAxis(90.0f, FVector(0, 0, 1)) * -1) * 75.0f);
	FVector Hit3EndPosition = InitialHitPosition + ((WorldDirection.RotateAngleAxis(90.0f, FVector(0, 0, 1)) * -1) * 75.0f);

	FHitResult Hit3 = TraceLine(Hit3StartPosition, Hit3EndPosition, false);
	if (Hit3.bBlockingHit)
	{
		AWNDoor* Door = Cast<AWNDoor>(Hit3.GetActor());
		if (Door)
		{
			return true;
		}
	}

	return !(Hit.bBlockingHit || Hit2.bBlockingHit || Hit3.bBlockingHit);
}

void Anpc_guard::Handle_RandomPatrol()
{
	FVector vecTraceEnd;
	FVector vecRotationDirection;

	if (this->testnext < GetGameTimeSinceCreation())
	{
		if (this->EMovementPriority == EEnemyMovementPriority::EMP_RIGHT)
		{
			vecTraceEnd = this->GetActorRightVector();
			vecRotationDirection = FVector(0, 90.0f, 0);

			if (this->CanMoveInDirection(vecTraceEnd, 150.0f))
			{
				//Can Move to the right
				this->SetActorRotation(this->GetActorRotation().RotateVector(vecRotationDirection).ToOrientationQuat());
				this->AddMovementInput(this->GetActorForwardVector(), 1.0f);
				testnext = GetGameTimeSinceCreation() + 1.0f;
				return;
			}
		}
	}

	vecTraceEnd = this->GetActorForwardVector();

	if (this->CanMoveInDirection(vecTraceEnd, 150.0f))
	{
		this->AddMovementInput(this->GetActorForwardVector(), 1.0f);
		return;
	}

	if (this->testnext < GetGameTimeSinceCreation())
	{
		if (this->EMovementPriority == EEnemyMovementPriority::EMP_RIGHT)
		{
			vecTraceEnd = this->GetActorRightVector() * -1;
			vecRotationDirection = FVector(0, -90.0f, 0);

			if (this->CanMoveInDirection(vecTraceEnd, 150.0f))
			{
				//Can Move to the right
				this->SetActorRotation(this->GetActorRotation().RotateVector(vecRotationDirection).ToOrientationQuat());
				//				this->AddMovementInput(this->GetActorForwardVector(), 1.0f);
				testnext = GetGameTimeSinceCreation() + 1.0f;
				return;
			}
		}
	}
}

void Anpc_guard::Handle_FixedPatrol()
{

}