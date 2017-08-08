// Copyright 2016 Carsten Zarbock / Rebound-Software
/************************************************************************/
/* GameMode                                                             */
/************************************************************************/

#include "WhiteNoise.h"
#include "WNGameMode.h"
#include "WNPlayerController.h"
#include "WNCharacter.h"
#include "WNNPC.h"
#include "WNTrigger.h"

AWhiteNoiseGameMode::AWhiteNoiseGameMode()
{
	PlayerControllerClass = AWhiteNoisePlayerController::StaticClass();
	this->bNeedStageInit = true;
	this->StageHandler = new StageHandle();
}

AWhiteNoiseGameMode::~AWhiteNoiseGameMode()
{
	delete this->StageHandler;
}

bool AWhiteNoiseGameMode::RestartStage()
{
	this->bNeedStageInit = false;

	this->StageHandler->EraseStage(GetWorld(), this->CurrentStage);
	this->StageHandler->RespawnEntities(GetWorld(), this->CurrentStage);
	this->StageHandler->RespawnPlayer(GetWorld());
	return true;
}

bool AWhiteNoiseGameMode::Register(AActor* TargetActor, bool bIsGarbage = false)
{
	if (bIsGarbage || this->bNeedStageInit)
	{
		this->StageHandler->Register(TargetActor, this->CurrentStage, bIsGarbage);

		this->RegisterAliveNPC(TargetActor);
		return true;
	}

	return false;
}

bool AWhiteNoiseGameMode::RegisterPlayer(APawn* PlayerCharacter)
{
	if (this->bNeedStageInit)
	{
		return this->StageHandler->RegisterPlayer(PlayerCharacter);
	}

	return false;
}

bool AWhiteNoiseGameMode::UpdatePlayer(APawn* PlayerCharacter)
{
	return this->StageHandler->UpdatePlayer(PlayerCharacter);
}

bool AWhiteNoiseGameMode::RegisterAliveNPC(AActor* TargetActor)
{
	ANPC* TargetNPC = Cast<ANPC>(TargetActor);

	if(TargetNPC)
	{
		AliveNPCs.Add(TargetActor);
		return true;
	}

	return false;
}

bool AWhiteNoiseGameMode::RemoveAliveNPC(AActor* TargetActor)
{
	ANPC* TargetNPC = Cast<ANPC>(TargetActor);

	if (TargetNPC)
	{
		if (this->AliveNPCs.RemoveSingle(TargetActor))
		{
			//todo: Diffent Stage / Floor goals, currently only ends if all registered NPCs are dead
			/* Check for Stage complete */
			if (this->AliveNPCs.Num() == 0)
			{
				/* Stage is complete */
				for (TActorIterator<AWNTrigger> ActorItr(GetWorld()); ActorItr; ++ActorItr)
				{
					AWNTrigger* TargetStart = *ActorItr;
					FName TargetTag;

					FString Temp1 = "Stagecomplete_";
					Temp1.AppendInt(this->CurrentStage);
					TargetTag = FName(*Temp1);

					if (TargetStart->IsTrigger(TargetTag))
					{
						TargetStart->SetActive(true);
					}
				}
			}

			return true;
		}
	}

	return false;
}

bool AWhiteNoiseGameMode::IncreaseStage()
{
	this->bNeedStageInit = true;
	this->CurrentStage++;
	this->StageHandler->IncreaseStage();
	return false;
}

AActor* AWhiteNoiseGameMode::FindPlayerStart(AController* Player, const FString &IncomingName)
{
	for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APlayerStart* TargetStart = *ActorItr;
		if (TargetStart->PlayerStartTag == "INITSTART")
		{
			return TargetStart;
		}
	}

	return nullptr;
}

/*APawn* AWhiteNoiseGameMode::SpawnDefaultPawnFor(AController* NewPlayer, AActor* StartSpot)
{
	UE_LOG(LogTemp, Warning, TEXT("AWhiteNoiseGameMode::SpawnDefaultPawnFor"));
	return nullptr;
}*/