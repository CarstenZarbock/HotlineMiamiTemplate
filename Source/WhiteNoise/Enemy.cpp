// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "Enemy.h"
#include "Engine.h"
#include "WhiteNoiseCharacter.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//this->GetMesh()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
	this->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
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
	MeshArmLeft->SetMasterPoseComponent(this->GetMesh());
	MeshArmRight->SetMasterPoseComponent(this->GetMesh());
	MeshHead->SetMasterPoseComponent(this->GetMesh());
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

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
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

	/* Set Stats */
	this->GetStats()->SetIsAlive(true);
	this->GetStats()->SetHealh(this->GetMaxHealth());
	this->GetStats()->SetIsCrawling(false);
	
	this->ChangeMovementState(EEnemyMovementState::EMS_WALK);
	this->ECurrentAnimation = ETargetAnimation::TA_NORMAL;
	this->SpawnStartWeapon();

}

// Called every frame
void AEnemy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	this->HandleAI();
}

bool AEnemy::PlayerInSight()
{
	//todo: Multiplayer
	AWhiteNoiseCharacter* PlayerCharacter = Cast<AWhiteNoiseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerCharacter != nullptr)
	{
		FHitResult Hit = TraceLine(this->GetActorLocation(), PlayerCharacter->GetActorLocation(), false);
		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor() == PlayerCharacter)
			{
				return true;
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "[AI ERROR] PLAYER CHARACTER INVALID");
	}

	return false;
}

void AEnemy::HandleAI()
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
					this->HandleAITypes();
				}
			}
		}
	}
}

void AEnemy::HandleCrawl()
{
	if (!this->GetCrawlState()->GetIsMovingToPoint())
	{
		/* He's not crawling to a target point */
		FVector vecDestination = this->GetRandomWalkpoint(false, 5000.0f); //todo fix range;
		this->GetCrawlState()->SetTargetPoint(vecDestination);
		this->GetCrawlState()->SetIsMovingToPoint(this->WalkToLocation(vecDestination));
	}
	else
	{
		/* we're crawling to a position, check if we reached, set to false */
		if (FVector::Dist(this->GetActorLocation(), this->GetCrawlState()->GetTargetPoint()) < 300.0f) //todo: Reach Distance
		{
			this->GetCrawlState()->SetIsMovingToPoint(false);
		}
	}
}

void AEnemy::HandleAITypes()
{
	if (this->GetEnemyType() == EEnemyMovement::EM_IDLE)
	{
		//He's not patroling, do nothing
	}
	else if (this->GetEnemyType() == EEnemyMovement::EM_RANDOMPATROL)
	{
		/* todo: fix this mess */
		//He's on random patrol
		FHitResult Hit = this->TraceLine(this->GetActorLocation(), this->GetActorLocation() + this->GetActorForwardVector() * 150, false);

		if (Hit.bBlockingHit)
		{
			if (this->GetMovementPriority() == EEnemyMovementPriority::EMP_LEFT)
			{
				Hit = this->TraceLine(this->GetActorLocation(), this->GetActorLocation() + (this->GetActorRightVector() * -1) * 150, false);
				if (Hit.bBlockingHit)
				{
					Hit = this->TraceLine(this->GetActorLocation(), this->GetActorLocation() + (this->GetActorRightVector()) * 150, false);
					if (Hit.bBlockingHit)
					{
						FRotator ActorRotation = this->GetActorRotation();

						ActorRotation.Yaw += 180;

						this->SetActorRotation(ActorRotation);
					}
					else
					{
						FRotator ActorRotation = this->GetActorRotation();

						ActorRotation.Yaw += 90;

						this->SetActorRotation(ActorRotation);
					}
				}
				else
				{
					FRotator ActorRotation = this->GetActorRotation();

					ActorRotation.Yaw -= 90;

					this->SetActorRotation(ActorRotation);
				}
			}
		}
		
		this->AddMovementInput(this->GetActorForwardVector(), 1.0f);
	}
	else if (this->GetEnemyType() == EEnemyMovement::EM_FIXEDPATROL)
	{
		//He got fixed waypoints
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "AEnemy");
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::FromInt(NormalImpulse.Size()));
}

void AEnemy::DamageApply(int iDamageAmount, FVector vecDirection)
{
	if (this->GetStats()->GetIsAlive())
	{
		if (iDamageAmount <= 50)
		{
			/* set crawling */
			this->GetStats()->SetHealh(this->GetStats()->GetHealth() - iDamageAmount);
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

void AEnemy::CutLimb(USkeletalMeshComponent* LimbMesh)
{
	if (LimbMesh != nullptr && LimbMesh->IsValidLowLevel())
	{
		FCollisionResponseContainer ColContainer;
		ColContainer.WorldStatic = 2;
		ColContainer.WorldDynamic = 0;
		ColContainer.Pawn = 0;
		ColContainer.PhysicsBody = 0;

		LimbMesh->SetMasterPoseComponent(nullptr);
		LimbMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		LimbMesh->SetCollisionObjectType(ECC_PhysicsBody);
		LimbMesh->SetSimulatePhysics(true);
		LimbMesh->SetCollisionResponseToChannels(ColContainer);
		LimbMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}
void AEnemy::ActivateGoreParticles(ECharacterLimbs ECutLimb)
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
void AEnemy::ExecuteDeathAnimation(FVector vecDirection)
{
	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (FMath::RandBool())
	{
		this->CutLimb(MeshHead);
		MeshHead->AddImpulseAtLocation(vecDirection, MeshHead->GetComponentLocation());
		this->ActivateGoreParticles(ECharacterLimbs::LIMB_HEAD);
	}

	if (FMath::RandBool())
	{
		this->CutLimb(MeshArmLeft);
		MeshArmLeft->AddImpulseAtLocation(vecDirection, MeshArmLeft->GetComponentLocation());
		this->ActivateGoreParticles(ECharacterLimbs::LIMB_ARMLEFT);
	}

	if (FMath::RandBool())
	{
		this->CutLimb(MeshArmRight);
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

void AEnemy::ExecuteDeathFullsplat(FVector vecDirection)
{
	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	this->CutLimb(MeshHead);
	MeshHead->AddImpulseAtLocation(vecDirection, MeshHead->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_HEAD);

	this->CutLimb(MeshArmLeft);
	MeshArmLeft->AddImpulseAtLocation(vecDirection, MeshArmLeft->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_ARMLEFT);

	this->CutLimb(MeshArmRight);
	MeshArmRight->AddImpulseAtLocation(vecDirection, MeshArmRight->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_ARMRIGHT);

	this->CutLimb(MeshTorso);
	MeshTorso->AddImpulseAtLocation(vecDirection, MeshTorso->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_TORSO);

	this->CutLimb(MeshLegLeft);
	MeshLegLeft->AddImpulseAtLocation(vecDirection, MeshLegLeft->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_LEGLEFT);

	this->CutLimb(MeshLegRight);
	MeshLegRight->AddImpulseAtLocation(vecDirection, MeshLegRight->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_LEGRIGHT);

	this->CutLimb(this->GetMesh()); //Lower Torso
	this->GetMesh()->AddImpulseAtLocation(vecDirection, this->GetMesh()->GetComponentLocation());
	this->ActivateGoreParticles(ECharacterLimbs::LIMB_BOTTOM);
}

void AEnemy::ChangeState(ETargetEnemyState ENewState)
{
	switch (ENewState)
	{
	case ETargetEnemyState::TES_CRAWL:
		this->GetStats()->SetIsCrawling(true);
		this->ChangeMovementState(EEnemyMovementState::EMS_CRAWL);
		break;
	case ETargetEnemyState::TES_DEAD:
		this->GetStats()->SetIsAlive(false);
		break;
	}
}

void AEnemy::ChangeMovementState(EEnemyMovementState ENewState)
{
	switch (ENewState)
	{
	case EEnemyMovementState::EMS_WALK:
		this->GetCharacterMovement()->MaxWalkSpeed = this->GetMovement()->GetWalkSpeed();
		break;
	case EEnemyMovementState::EMS_RUN:
		this->GetCharacterMovement()->MaxWalkSpeed = this->GetMovement()->GetRunSpeed();
		break;
	case EEnemyMovementState::EMS_CRAWL:
		this->GetCharacterMovement()->MaxWalkSpeed = this->GetMovement()->GetCrawlSpeed();
		break;
	default:
		break;
	}
}

FVector AEnemy::GetRandomWalkpoint(bool inRange, float Range)
{
	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
	FNavLocation Location;

	if (NavSys != nullptr)
	{
		if (inRange)
		{
			NavSys->GetRandomReachablePointInRadius(this->GetActorLocation(), Range, Location);
		}
		else
		{
			NavSys->GetRandomPoint(Location, NavSys->MainNavData);
		}
	}

	return Location.Location;
}

bool AEnemy::WalkToLocation(FVector vecDestinationLocation)
{
	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();

	if (NavSys != nullptr)
	{
		NavSys->SimpleMoveToLocation(this->GetController(), vecDestinationLocation);
		return true;
	}

	return false;
}

void AEnemy::SpawnStartWeapon()
{
	if (this->AStarterWeapon != nullptr)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		SpawnTransform.SetLocation(this->GetActorLocation());
		SpawnTransform.SetRotation(this->GetActorRotation().Quaternion());
		AWeapon* SpawnedActor = Cast<AWeapon>(GetWorld()->SpawnActor(this->AStarterWeapon));

		if (SpawnedActor != nullptr)
		{
			SpawnedActor->SetActorLocation(this->GetActorLocation());
			this->Weapon_Pickup(SpawnedActor);
		}
	}
}

void AEnemy::Weapon_Drop()
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


bool AEnemy::Weapon_Pickup(AWeapon* TargetWeaponActor)
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

/* ------------------------------------------
* TraceLine
* @Param FVector Start - Start position world space
* @Param FVector End - End position world space
* @Param bool Debug - Draw Debug Lines
* Casts a trace line, returns hitresult
*/
FHitResult AEnemy::TraceLine(FVector Start, FVector End, bool Debug)
{
	//todo: rework function
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = true;

	if (Debug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		RV_TraceParams.TraceTag = TraceTag;
	}

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,		       //result
		Start,			  //start
		End,			 //end
		ECC_Visibility, //collision channel
		RV_TraceParams
	);

	if (Debug)
	{
		DrawDebugLine(
			GetWorld(),
			Start,
			RV_Hit.ImpactPoint,
			FColor(255, 0, 0),
			false, -1, 0,
			12.333
		);
	}

	return RV_Hit;
}