// Copyright 2016 Carsten Zarbock / Rebound-Software
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

	/* Delete all NPCs */
	this->StageHandler->EraseStage(GetWorld(), this->CurrentStage);

	/* Respawn all NPCs */
	this->StageHandler->RespawnEntities(GetWorld(), this->CurrentStage);

	return true;
}

bool AWhiteNoiseGameMode::Register(AActor* TargetActor, bool bIsGarbage = false)
{
	UE_LOG(LogTemp, Warning, TEXT("Gamemod register1 : %s"), *TargetActor->GetName());
	if (bIsGarbage || this->bNeedStageInit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Gamemod register2 : %s"), *TargetActor->GetName());
		this->StageHandler->Register(TargetActor, this->CurrentStage, bIsGarbage);

		this->RegisterAliveNPC(TargetActor);
		return true;
	}

	return false;
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
	UE_LOG(LogTemp, Warning, TEXT("AWhiteNoiseGameMode::FindPlayerStart"));

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