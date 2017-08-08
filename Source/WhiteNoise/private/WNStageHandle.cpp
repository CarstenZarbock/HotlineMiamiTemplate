// Copyright 2016 Carsten Zarbock / Rebound-Software

/************************************************************************/
/* StageHandle is used to instant restart the current stage / floor,	*/
/* instead of a full map reload.										*/
/* TArray for multiple stages is NOT needed                             */
/* Todo: Replace with single stage to save memory						*/
/************************************************************************/
#include "WhiteNoise.h"
#include "WNStageHandle.h"
#include "WNCharacter.h"
#include "WNWeapon.h"

StageHandle::StageHandle()
{
	this->PlayerCharacter = new FActorSaveData();
	this->PlayerItem = new FActorSaveData();
}

StageHandle::~StageHandle()
{
	delete this->PlayerCharacter;
	delete this->PlayerItem;
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

bool StageHandle::RegisterPlayer(APawn* PlayerCharacter)
{
	/* Pawn Itself */
	FActorSaveData ActorRecord;
	ActorRecord.ActorName = FName(*PlayerCharacter->GetName());
	ActorRecord.ActorClass = PlayerCharacter->GetClass()->GetPathName();
	ActorRecord.ActorTransform = PlayerCharacter->GetActorTransform();
	ActorRecord.LastActor = PlayerCharacter;

	FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
	FSaveGameArchive Ar(MemoryWriter);
	PlayerCharacter->Serialize(Ar);

	delete this->PlayerCharacter;
	this->PlayerCharacter = new FActorSaveData(ActorRecord);

	/* Equipped Weapon */
	AWhiteNoiseCharacter* WNPlayerCharacter = Cast<AWhiteNoiseCharacter>(PlayerCharacter);
	if (WNPlayerCharacter != nullptr)
	{
		if (WNPlayerCharacter->hasItemEquipped())
		{
			FActorSaveData ActorRecord;
			ActorRecord.ActorName = FName(*WNPlayerCharacter->GetItemName());
			ActorRecord.ActorClass = WNPlayerCharacter->GetItemClass()->GetPathName();
			ActorRecord.ActorTransform = WNPlayerCharacter->GetItemTransform();
			ActorRecord.LastActor = WNPlayerCharacter->CurrentWeapon;

			FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
			FSaveGameArchive Ar(MemoryWriter);
			WNPlayerCharacter->CurrentWeapon->Serialize(Ar);

			delete this->PlayerItem;
			this->PlayerItem = new FActorSaveData(ActorRecord);
		}
		else
		{
			this->PlayerItem = nullptr;
		}
	}

	if (this->PlayerCharacter != nullptr) { return true; }

	return false;
}

bool StageHandle::UpdatePlayer(APawn* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("StageHandle::Update Player"));

	/* Pawn Itself */
	FActorSaveData ActorRecord;
	ActorRecord.ActorName = FName(*PlayerCharacter->GetName());
	ActorRecord.ActorClass = PlayerCharacter->GetClass()->GetPathName();
	ActorRecord.ActorTransform = PlayerCharacter->GetActorTransform();
	ActorRecord.LastActor = PlayerCharacter;

	FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
	FSaveGameArchive Ar(MemoryWriter);
	PlayerCharacter->Serialize(Ar);

	delete this->PlayerCharacter;
	this->PlayerCharacter = new FActorSaveData(ActorRecord);

	/* Equipped Item */
	AWhiteNoiseCharacter* WNPlayerCharacter = Cast<AWhiteNoiseCharacter>(PlayerCharacter);
	if (WNPlayerCharacter != nullptr)
	{
		if (WNPlayerCharacter->hasItemEquipped())
		{
			FActorSaveData ActorRecord;
			ActorRecord.ActorName = FName(*WNPlayerCharacter->GetItemName());
			ActorRecord.ActorClass = WNPlayerCharacter->GetItemClass()->GetPathName();
			ActorRecord.ActorTransform = WNPlayerCharacter->GetItemTransform();
			ActorRecord.LastActor = WNPlayerCharacter->CurrentWeapon;
			WNPlayerCharacter->CurrentWeapon->MarkAsGarbage();
			WNPlayerCharacter->CurrentWeapon->UpdateAsGarbage();
			WNPlayerCharacter->CurrentWeapon->bIsRestartWeapon = true;

			FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
			FSaveGameArchive Ar(MemoryWriter);
			WNPlayerCharacter->CurrentWeapon->Serialize(Ar);

			delete this->PlayerItem;
			this->PlayerItem = new FActorSaveData(ActorRecord);
		}
		else
		{
			this->PlayerItem = nullptr;
		}
	}

	if (this->PlayerCharacter != nullptr) { return true; }

	return false;
}

bool StageHandle::UpdateActorAsGarbage(AActor* TargetActor, int32 StageID)
{
	/** Remove from non garbage list */
	int32 StageIndex = this->CheckStageArraySize(StageID);

	if (StageIndex == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("WARNING: Invalid StageID detected ( %d ) StageHandle::Register"), StageIndex);
		return false;
	}

	for (int i = 0; i < this->Stages[StageIndex].SpawnEntity.Num(); i++)
	{
		if (this->Stages[StageIndex].SpawnEntity[i].LastActor == TargetActor)
		{
			this->Stages[StageIndex].SpawnEntity.RemoveAt(i);
			break;
		}
	}

	/** Register again as garbage */
	return this->Register(TargetActor, StageID, true);
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

	/* Erase Player :: Split Function? */
	if (this->PlayerCharacter->LastActor != nullptr)
	{
		this->PlayerCharacter->LastActor->Destroy();
		this->PlayerCharacter->LastActor = nullptr;
	}
}

void StageHandle::RespawnPlayer(UWorld* World)
{
	FActorSaveData* ActorEntity = this->PlayerCharacter;
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

		/* Possess new pawn */
		GWorld->GetFirstPlayerController()->Possess(Cast<APawn>(NewActor));

		/* Equipped item */
		if (this->PlayerItem != nullptr)
		{

			ActorEntity = this->PlayerItem;
			SpawnTrans = ActorEntity->ActorTransform;
			SpawnParams;
			SpawnParams.Name = ActorEntity->ActorName;

			SpawnClass = FindObject<UClass>(ANY_PACKAGE, *ActorEntity->ActorClass);
			if (SpawnClass)
			{
				AActor* NewItemActor = World->SpawnActorDeferred<AActor>(SpawnClass, SpawnTrans);
				AWeapon* NewWeaponActor = Cast<AWeapon>(NewItemActor);

				ActorEntity->LastActor = NewItemActor;
				FMemoryReader MemoryReader(ActorEntity->ActorData, true);
				FSaveGameArchive Ar(MemoryReader);
				NewItemActor->Serialize(Ar);

				//Deferred ignores the BP Constructor, be sure to apply the BP Class transform				
				NewItemActor->SetActorTransform(SpawnTrans, false, nullptr, ETeleportType::TeleportPhysics);

				if (NewWeaponActor != nullptr)
				{
					NewWeaponActor->MarkAsGarbage();
				}

				NewItemActor->FinishSpawning(SpawnTrans);

				AWhiteNoiseCharacter* PlayerChar = Cast<AWhiteNoiseCharacter>(NewActor);
				if (PlayerChar != nullptr && NewWeaponActor != nullptr)
				{
					PlayerChar->ForceWeaponEquip(NewWeaponActor);
				}
			}
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