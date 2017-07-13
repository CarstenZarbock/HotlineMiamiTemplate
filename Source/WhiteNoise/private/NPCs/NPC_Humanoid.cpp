// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "NPC_Humanoid.h"

AHumanoid::AHumanoid()
{
	MeshTorso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Torso"));
	MeshTorso->SetupAttachment(this->GetMesh());
	MeshArmLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmLeft"));
	MeshArmLeft->SetupAttachment(MeshTorso);
	MeshArmRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmRight"));
	MeshArmRight->SetupAttachment(MeshTorso);
	MeshHead = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	MeshHead->SetupAttachment(MeshTorso);
	MeshLegLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegLeft"));
	MeshLegLeft->SetupAttachment(this->GetMesh());
	MeshLegRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegRight"));
	MeshLegRight->SetupAttachment(this->GetMesh());

	MeshTorso->SetMasterPoseComponent(this->GetMesh());
	MeshArmLeft->SetMasterPoseComponent(this->MeshTorso);
	MeshArmRight->SetMasterPoseComponent(this->MeshTorso);
	MeshHead->SetMasterPoseComponent(this->MeshTorso);
	MeshLegLeft->SetMasterPoseComponent(this->GetMesh());
	MeshLegRight->SetMasterPoseComponent(this->GetMesh());

	ArmLeftGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ArmLeftGore"));
	ArmLeftGore->SetupAttachment(MeshArmLeft);
	ArmRightGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ArmRightGore"));
	ArmRightGore->SetupAttachment(MeshArmRight);
	HeadGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HeadGore"));
	HeadGore->SetupAttachment(MeshHead);
	LegRightGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LegRightGore"));
	LegRightGore->SetupAttachment(MeshLegRight);
	LegLeftGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LegLeftGore"));
	LegLeftGore->SetupAttachment(MeshLegLeft);

	TorsoHeadGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TorsoHeadGore"));
	TorsoHeadGore->SetupAttachment(MeshTorso);
	TorsoBottomGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TorsoBottomGore"));
	TorsoBottomGore->SetupAttachment(MeshTorso);
	TorsoArmLeftGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TorsoArmLeftGore"));
	TorsoArmLeftGore->SetupAttachment(MeshTorso);
	TorsoArmRightGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TorsoArmRightGore"));
	TorsoArmRightGore->SetupAttachment(MeshTorso);

	BottomTorsoGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BottomTorsoGore"));
	BottomTorsoGore->SetupAttachment(this->GetMesh());
	BottomLeftLegGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BottomLeftLegGore"));
	BottomLeftLegGore->SetupAttachment(this->GetMesh());
	BottomRightLegGore = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BottomRightLegGore"));
	BottomRightLegGore->SetupAttachment(this->GetMesh());

	WeaponGripPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponGripPoint"));
	WeaponGripPoint->SetupAttachment(this->GetMesh());
}

void AHumanoid::BeginPlay()
{
	Super::BeginPlay();
	ArmLeftGore->AttachToComponent(MeshArmLeft, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LeftArm"));
	ArmRightGore->AttachToComponent(MeshArmRight, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightArm"));
	HeadGore->AttachToComponent(MeshHead, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Head"));
	LegRightGore->AttachToComponent(MeshLegRight, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightLeg"));
	LegLeftGore->AttachToComponent(MeshLegLeft, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LeftLeg"));

	TorsoHeadGore->AttachToComponent(MeshTorso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Neck"));
	TorsoBottomGore->AttachToComponent(MeshTorso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Hips"));
	TorsoArmLeftGore->AttachToComponent(MeshTorso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LeftShoulder"));
	TorsoArmRightGore->AttachToComponent(MeshTorso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightShoulder"));
	BottomTorsoGore->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Hips"));
	BottomLeftLegGore->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LeftUpLeg"));
	BottomRightLegGore->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightUpLeg"));

	WeaponGripPoint->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightHand"));
	FVector bla = { -90.0f, 150.0f, 80.0f };
	WeaponGripPoint->SetRelativeRotation(FQuat::MakeFromEuler(bla));
	
	this->ECurrentAnimation = ETargetAnimation::TA_NORMAL;
	this->SpawnStartWeapon();
}

void AHumanoid::HandleAI()
{

}

void AHumanoid::HandleCrawl()
{
	if (!this->bIsMovingToPoint)
	{
		/* He's not crawling to a target point */
		FVector vecDestination = this->GetRandomWalkpoint(true, 1000.0f); //todo fix range;
		this->bIsMovingToPoint = this->WalkToLocation(vecDestination);
		this->vecTargetPoint = vecDestination;
	}
	else
	{
		/* we're crawling to a position, check if we reached, set to false */
		if (FVector::Dist(this->GetActorLocation(), this->vecTargetPoint) < 300.0f) //todo: Reach Distance
		{
			this->bIsMovingToPoint = false;
		}
	}
}

void AHumanoid::SpawnStartWeapon()
{
	if (this->AStarterWeapon != nullptr)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		SpawnTransform.SetLocation(this->GetActorLocation());
		SpawnTransform.SetRotation(this->GetActorRotation().Quaternion());
		//AWeapon* SpawnedActor = Cast<AWeapon>(GetWorld()->SpawnActor(this->AStarterWeapon));
		
		AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(this->AStarterWeapon, SpawnTransform);
		AWeapon* NewWeapon = Cast<AWeapon>(NewActor);

		if (NewWeapon != nullptr)
		{
			NewWeapon->MarkAsGarbage();

			NewWeapon->FinishSpawning(SpawnTransform);
			NewWeapon->SetActorLocation(this->GetActorLocation());
			this->Weapon_Pickup(NewWeapon);
		}

		//Deferred ignores the BP Constructor, be sure to apply the BP Class transform				
		//NewActor->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AHumanoid::Weapon_Drop()
{
	if (this->GetWeapon() != nullptr && this->GetWeapon()->IsValidLowLevel())
	{
		this->GetWeapon()->DetachRootComponentFromParent(true);
		FVector vecNewLocation = this->GetActorLocation() + (this->GetActorForwardVector() * 150);
		this->GetWeapon()->SetActorLocation(vecNewLocation);
		this->GetWeapon()->ChangeState(EWeaponState::WS_WORLD);
		this->SetWeapon(nullptr);
		this->ECurrentAnimation = ETargetAnimation::TA_NORMAL;
	}
}

bool AHumanoid::Weapon_Pickup(AWeapon* TargetWeaponActor)
{
	if (this->GetWeapon() == nullptr)
	{
		TargetWeaponActor->ChangeState(EWeaponState::WS_EQUIP);
		this->SetWeapon(TargetWeaponActor);
		TargetWeaponActor->AttachToComponent(this->WeaponGripPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		this->ECurrentAnimation = TargetWeaponActor->GetTargetAnimation();
		return true;
	}
	else
	{
		/* drop current weapon */
		this->Weapon_Drop();
	}
	return false;
}

void AHumanoid::DamageApply(int iDamageAmount, FVector vecDirection)
{
	if (this->bIsAlive)
	{
		if (iDamageAmount <= 50)
		{
			/* set crawling */
			this->Health -= iDamageAmount;
			this->ChangeState(ETargetEnemyState::TES_CRAWL);
			if (this->GetWeapon() != nullptr && this->GetWeapon()->IsValidLowLevel())
			{
				this->Weapon_Drop();
			}
		}
		else if (iDamageAmount > 50 && iDamageAmount <= 100)
		{
			/* Animation death */
			this->ExecuteDeathAnimation(vecDirection);
			this->ChangeState(ETargetEnemyState::TES_DEAD);
			if (this->GetWeapon() != nullptr && this->GetWeapon()->IsValidLowLevel())
			{
				this->Weapon_Drop();
			}
		}
		else
		{
			/* splatter */
			this->ExecuteDeathFullsplat(vecDirection);
			this->ChangeState(ETargetEnemyState::TES_DEAD);
			if (this->GetWeapon() != nullptr && this->GetWeapon()->IsValidLowLevel())
			{
				this->Weapon_Drop();
			}
		}
	}
}

void AHumanoid::CutLimb(USkeletalMeshComponent* LimbMesh, const bool bHardCut)
{
	if (LimbMesh != nullptr && LimbMesh->IsValidLowLevel())
	{
		FCollisionResponseContainer ColContainer;
		ColContainer.WorldStatic = 2;
		ColContainer.WorldDynamic = 0;
		ColContainer.Pawn = 0;
		ColContainer.PhysicsBody = 0;

		LimbMesh->SetMasterPoseComponent(nullptr);
		LimbMesh->SetCollisionResponseToChannels(ColContainer);
		LimbMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		LimbMesh->SetCollisionObjectType(ECC_PhysicsBody);
		LimbMesh->SetSimulatePhysics(true);
	
		/* Todo: Soft Cut is not working right now. Attach single sockets to fix? */
		//if (bHardCut)
		//{
			LimbMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		//}
	}
}

void AHumanoid::ActivateGoreParticles(ECharacterLimbs ECutLimb)
{
	switch (ECutLimb)
	{
	case ECharacterLimbs::LIMB_HEAD:
		HeadGore->ActivateSystem();
		TorsoHeadGore->ActivateSystem();
		break;
	case ECharacterLimbs::LIMB_TORSO:
		TorsoBottomGore->ActivateSystem();
		BottomTorsoGore->ActivateSystem();
		break;
	case ECharacterLimbs::LIMB_ARMLEFT:
		ArmLeftGore->ActivateSystem();
		TorsoArmLeftGore->ActivateSystem();
		break;
	case ECharacterLimbs::LIMB_ARMRIGHT:
		ArmRightGore->ActivateSystem();
		TorsoArmRightGore->ActivateSystem();
		break;
	case ECharacterLimbs::LIMB_BOTTOM:
		BottomTorsoGore->ActivateSystem();
		TorsoBottomGore->ActivateSystem();
		break;
	case ECharacterLimbs::LIMB_LEGLEFT:
		LegLeftGore->ActivateSystem();
		BottomLeftLegGore->ActivateSystem();
		break;
	case ECharacterLimbs::LIMB_LEGRIGHT:
		LegRightGore->ActivateSystem();
		BottomRightLegGore->ActivateSystem();
		break;
	default:
		break;
	}
}

void AHumanoid::ExecuteDeathFullsplat(FVector vecDirection)
{
	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	this->CutLimb(MeshHead, false);
	MeshHead->AddImpulseAtLocation(vecDirection, MeshHead->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_HEAD);

	this->CutLimb(MeshArmLeft, false);
	MeshArmLeft->AddImpulseAtLocation(vecDirection, MeshArmLeft->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_ARMLEFT);

	this->CutLimb(MeshArmRight, false);
	MeshArmRight->AddImpulseAtLocation(vecDirection, MeshArmRight->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_ARMRIGHT);

	this->CutLimb(MeshTorso, true);
	MeshTorso->AddImpulseAtLocation(vecDirection, MeshTorso->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_TORSO);

	this->CutLimb(MeshLegLeft, false);
	MeshLegLeft->AddImpulseAtLocation(vecDirection, MeshLegLeft->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_LEGLEFT);

	this->CutLimb(MeshLegRight, false);
	MeshLegRight->AddImpulseAtLocation(vecDirection, MeshLegRight->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_LEGRIGHT);

	this->CutLimb(this->GetMesh(), true); //Lower Torso
	this->GetMesh()->AddImpulseAtLocation(vecDirection, this->GetMesh()->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_BOTTOM);
}

void AHumanoid::ExecuteDeathAnimation(FVector vecDirection)
{
	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (FMath::RandBool())
	{
		this->CutLimb(MeshHead, true);
		MeshHead->AddImpulseAtLocation(vecDirection, MeshHead->GetComponentLocation());
		this->ActivateGoreParticles(ECharacterLimbs::LIMB_HEAD);
	}

	if (FMath::RandBool())
	{
		this->CutLimb(MeshArmLeft, true);
		MeshArmLeft->AddImpulseAtLocation(vecDirection, MeshArmLeft->GetComponentLocation());
		this->ActivateGoreParticles(ECharacterLimbs::LIMB_ARMLEFT);
	}

	if (FMath::RandBool())
	{
		this->CutLimb(MeshArmRight, true);
		MeshArmRight->AddImpulseAtLocation(vecDirection, MeshArmRight->GetComponentLocation());
		this->ActivateGoreParticles(ECharacterLimbs::LIMB_ARMRIGHT);
	}

	/*if (FMath::RandBool())
	{
	this->CutLimb(MeshTorso);
	MeshTorso->AddImpulseAtLocation(vecDirection, MeshTorso->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_TORSO);
	}*/

	this->GetMesh()->PlayAnimation(DeathAnimation_1, false);
}