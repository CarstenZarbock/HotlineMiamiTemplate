// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "npc_guard.h"

/* basic enemy npc, can carry weapon, can patrol around */
Anpc_guard::Anpc_guard()
{

}

void Anpc_guard::HandleAI()
{
	if (this->GetStats()->GetIsAlive())
	{
		if (this->GetStats()->GetIsCrawling())
		{
			this->HandleCrawl();
		}
		else
		{
			if (this->GetAIState()->GetHasSeenPlayer())
			{
				/* Player has been seen*/
				if (this->PlayerInSight())
				{
					//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, "[AI] PLAYER IN SIGHT");
				}
				else
				{
					/* Player is not insight anymore */
					this->GetAIState()->SetHasSeenPlayer(false);

				}
			}
			else
			{
				if (this->PlayerInSight())
				{
					/* Set Player is in Sight, Handle next Tick*/
					this->GetAIState()->SetHasSeenPlayer(true);
					return;
				}
				else
				{
					/* Default Action, Path Walking, etc*/
					//this->HandleAITypes();
				}
			}
		}
	}
}
