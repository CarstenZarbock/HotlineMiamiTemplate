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

/**
* Serialization of each actor, SaveGame properties are used
*/
USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

	FString ActorClass;
	FName ActorName;
	FTransform ActorTransform;
	TArray<uint8> ActorData;

	/** Reference to existing actor for clean up purpose */
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

/** Object content for each stage */
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
	/** Contains map placed actor objects of old and the current stage */
	TArray<FStage> Stages;

	/** Checks if given Stage ID does exist, creates one if it doesnt. Returns Array Index of the Stage */
	int32 CheckStageArraySize(int32 StageID);

	/** Contains all dynamic spawned actors while playing a stage, excludes actors placed on map. */
	TArray<AActor*> GarbageActors;

	FActorSaveData* PlayerCharacter;
	FActorSaveData* PlayerItem;

	/** Cleans up GarbageActors on the current stage */
	void EraseGarbage();

public:
	/** */
	StageHandle();

	/** */
	~StageHandle();

	/** */
	bool RegisterPlayer(APawn* PlayerCharacter);

	/** */
	bool UpdatePlayer(APawn* PlayerCharacter);

	/** Adds an actor to the given stage */
	bool Register(AActor* TargetActor, int32 StageID, bool bIsGarbage);

	/** Spawns all registered actors for this stage  */
	void RespawnEntities(UWorld* World, int32 StageID);

	/** */
	void RespawnPlayer(UWorld* World);

	/** Destroys all registered actors for this stage */
	void EraseStage(UWorld* World, int32 StageID);

	/** Called by GameMode when next stage is starting */
	void IncreaseStage();

	bool UpdateActorAsGarbage(AActor* TargetActor, int32 StageID);
};
