// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WNNPC.h"
#include "WNWeapon.h"
#include "WNStageHandle.generated.h"
#pragma once

/**
 * 
 */
struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true;
	}
};

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

	FString ActorClass;
	FName ActorName;
	FTransform ActorTransform;
	TArray<uint8> ActorData;
	AActor* LastActor;

	friend FArchive& operator<<(FArchive& Ar, FActorSaveData& ActorData)
	{
		Ar << ActorData.ActorClass;
		Ar << ActorData.ActorName;
		Ar << ActorData.ActorTransform;
		Ar << ActorData.ActorData;
		Ar << ActorData.LastActor;
		return Ar;
	}
};

struct FStage
{
	int32 StageID;
	TArray<FActorSaveData> SpawnEntity;

	FStage(int32 StageID)
	{
		this->StageID = StageID;
	}
};

class WHITENOISE_API StageHandle
{
private:
	TArray<FStage> Stages;
	int32 CheckStageArraySize(int32 StageID);
	TArray<AActor*> GarbageActors; //Dynamic spawned actors while instage
	void EraseGarbage();

public:
	StageHandle();
	~StageHandle();
	bool Register(AActor* TargetActor, int32 StageID, bool bIsGarbage);
	void RespawnEntities(UWorld* World, int32 StageID);
	void EraseStage(UWorld* World, int32 StageID);
	void IncreaseStage();
};
