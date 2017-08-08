// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once
#include "GameFramework/GameModeBase.h"
#include "WNStageHandle.h"
#include "WNGameMode.generated.h"

UCLASS(minimalapi)
class AWhiteNoiseGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:
	/** Management of the actors to reset */
	StageHandle* StageHandler;

	/** If true, Actors get registered at stage start. False after first reset */
	bool bNeedStageInit;

	/** Current stage, used by stage handler */
	uint32 CurrentStage;

	/** List of (hostile) NPCs used to check stage goal */
	TArray<AActor*> AliveNPCs;	


	/** Registers a NPC as alive to remember NPCs in world */
	bool RegisterAliveNPC(AActor* TargetActor);

public:
	/** */
	AWhiteNoiseGameMode();

	/** */
	~AWhiteNoiseGameMode();

	/** Registers an actor as in world, used by stage reset */
	bool Register(AActor* TargetActor, bool bIsGarbage);

	/** */
	bool UpdateActorAsGarbage(AActor* TargetActor);

	/** */
	bool RegisterPlayer(APawn* PlayerCharacter);

	/** */
	bool UpdatePlayer(APawn* PlayerCharacter);

	/** Deletes all actors and respawns them */
	bool RestartStage();

	/** */
	bool IncreaseStage();

	/** Removes NPC from the alive list | .num() == 0 => all dead */
	bool RemoveAliveNPC(AActor* TargetActor);
	
	/** */
	UFUNCTION()
	AActor* FindPlayerStart(AController* Player, const FString &IncomingName);

	/** */
	//UFUNCTION()
	//APawn* SpawnDefaultPawnFor(AController* NewPlayer, AActor* StartSpot);
};



