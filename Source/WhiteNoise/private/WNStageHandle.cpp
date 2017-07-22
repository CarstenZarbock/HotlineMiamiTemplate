// Copyright 2016 Carsten Zarbock / Rebound-Software

/************************************************************************/
/* StageHandle is used to instant restart the current stage / floor,	*/
/* instead of a full map reload.										*/
/* TArray for multiple stages is NOT needed                             */
/* Todo: Replace with single stage to save memory						*/
/************************************************************************/
#include "WhiteNoise.h"
#include "WNStageHandle.h"

StageHandle::StageHandle()
{
}

StageHandle::~StageHandle()
{
}

int32 StageHandle::CheckStageArraySize(int32 StageID)
{
	if (StageID < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WARNING: Invalid StageID detected ( %d ) StageHandle::CheckStageArraySize"), StageID);
		StageID = 0;
	}

	for (int32 i = 0; i < this->Stages.Num(); i++)
	{
		if (this->Stages[i].StageID == StageID)
		{
			return i;
		}
	}

	//Stage is not initialized yet
	int32 newIndex = this->Stages.Add(FStage(StageID));
	UE_LOG(LogTemp, Warning, TEXT("Amount of stages after StageHandle::CheckStageArraySize: %d"), this->Stages.Num());

	return newIndex;
}

bool StageHandle::Register(AActor* TargetActor, int32 StageID, bool bIsGarbage = false)
{
	if (bIsGarbage)
	{
		this->GarbageActors.Add(TargetActor);
		return true;
	}

	int32 StageIndex = this->CheckStageArraySize(StageID);

	if (StageIndex == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("WARNING: Invalid StageID detected ( %d ) StageHandle::Register"), StageIndex);
		return false;
	}

	FActorSaveData ActorRecord;
	ActorRecord.ActorName = FName(*TargetActor->GetName());
	ActorRecord.ActorClass = TargetActor->GetClass()->GetPathName();
	ActorRecord.ActorTransform = TargetActor->GetActorTransform();
	ActorRecord.LastActor = TargetActor;

	FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
	FSaveGameArchive Ar(MemoryWriter);
	TargetActor->Serialize(Ar);

	int32 EntityIndex = this->Stages[StageIndex].SpawnEntity.Add(ActorRecord);
	
	if (EntityIndex == -1)
	{
		return false;
	}

	return true;
}

void StageHandle::EraseGarbage()
{
	int32 GarbageActorCount = this->GarbageActors.Num();

	for (int32 i = 0; i < GarbageActorCount; i++)
	{
		if (this->GarbageActors[i]->IsValidLowLevel())
		{
			this->GarbageActors[i]->Destroy();
		}
	}

	this->GarbageActors.Empty();
}

void StageHandle::EraseStage(UWorld* World, int32 StageID)
{
	int32 StageCount = this->Stages.Num();
	int32 StageIndex = -1;

	for (int32 i = 0; i < StageCount; i++)
	{
		if (this->Stages[i].StageID == StageID)
		{
			StageIndex = i;
			break;
		}
	}

	//Erase Garbage Actors first before continue
	this->EraseGarbage();

	if (StageIndex == -1) //Stage does not exist / no entities in there
	{
		return;
	}
	else
	{
		int32 StageEntitiesCount = this->Stages[StageIndex].SpawnEntity.Num();

		for (int i = 0; i < StageEntitiesCount; i++)
		{
			FActorSaveData* ActorEntity = &this->Stages[StageIndex].SpawnEntity[i];
			if (ActorEntity->LastActor != nullptr && ActorEntity->LastActor->IsValidLowLevel())
			{
				ActorEntity->LastActor->Destroy();
			}

			ActorEntity->LastActor = nullptr;
		}
	}
}

void StageHandle::RespawnEntities(UWorld* World, int32 StageID)
{
	int32 StageCount = this->Stages.Num();
	int32 StageIndex = -1;

	for (int32 i = 0; i < StageCount; i++)
	{
		if (this->Stages[i].StageID == StageID)
		{
			StageIndex = i;
			break;
		}
	}

	if (StageIndex == -1) //Stage does not exist / no entities in there
	{
		return;
	}
	else
	{
		int32 StageEntitiesCount = this->Stages[StageIndex].SpawnEntity.Num();

		for (int i = 0; i < StageEntitiesCount; i++)
		{
			FActorSaveData* ActorEntity = &this->Stages[StageIndex].SpawnEntity[i];

			FTransform SpawnTrans = ActorEntity->ActorTransform;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Name = ActorEntity->ActorName;


			UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *ActorEntity->ActorClass);
			if (SpawnClass)
			{
				AActor* NewActor = World->SpawnActorDeferred<AActor>(SpawnClass, SpawnTrans);
				ActorEntity->LastActor = NewActor;

				FMemoryReader MemoryReader(ActorEntity->ActorData, true);
				FSaveGameArchive Ar(MemoryReader);
				NewActor->Serialize(Ar);
				
				//Deferred ignores the BP Constructor, be sure to apply the BP Class transform				
				NewActor->SetActorTransform(SpawnTrans, false, nullptr, ETeleportType::TeleportPhysics);

				NewActor->FinishSpawning(SpawnTrans);
			}
		}
	}
}

void StageHandle::IncreaseStage()
{
	this->GarbageActors.Empty();
	//Todo: Empty SpawnEntities
}