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
	/** */
	StageHandle* StageHandler;

	/** */
	bool bNeedStageInit;

	/** */
	uint32 CurrentStage;

	/** */
	TArray<AActor*> AliveNPCs;	


	/** */
	bool RegisterAliveNPC(AActor* TargetActor);

public:
	/** */
	AWhiteNoiseGameMode();

	/** */
	~AWhiteNoiseGameMode();

	/** */
	bool Register(AActor* TargetActor, bool bIsGarbage);

	/** */
	bool RestartStage();

	/** */
	bool IncreaseStage();

	/** */
	bool RemoveAliveNPC(AActor* TargetActor);
	
	/** */
	UFUNCTION()
	AActor* FindPlayerStart(AController* Player, const FString &IncomingName);

	/** */
	//UFUNCTION()
	//APawn* SpawnDefaultPawnFor(AController* NewPlayer, AActor* StartSpot);
};



