// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "WNCharacter.h"
#include "Door_SwingSingle.h"
#include "WNGameMode.h"

AWhiteNoiseCharacter::AWhiteNoiseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AWhiteNoiseCharacter::OnHit);
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false; //no character oritation to movement
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	WeaponThrowArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("WeaponThrowArm"));
	WeaponThrowArm->SetupAttachment(RootComponent);
	WeaponThrowArm->TargetArmLength = 150.0f;
	WeaponThrowArm->bDoCollisionTest = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // no collision test, celing gets ignored

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	WeaponGripPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponGripPoint"));
	WeaponGripPoint->SetupAttachment(this->GetMesh());

	this->CurrentWeapon = nullptr;
	this->LockedEnemy = nullptr;
}

void AWhiteNoiseCharacter::BeginPlay()
{
	Super::BeginPlay();
	this->RegisterOnGameMode();
	/* mesh(es) not initialised in constructor, attach at beginplay */
	WeaponGripPoint->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightHand"));
	
	/* reset relative rotation after attachement */
	WeaponGripPoint->SetRelativeRotation(FQuat::MakeFromEuler(FVector(-90.0f, 170.0f, 80.0f))); //todo: Better solution?
}

bool AWhiteNoiseCharacter::RegisterOnGameMode()
{
	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	if (GMBase != nullptr)
	{
		AWhiteNoiseGameMode* GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
		if (GMWNBase != nullptr)
		{
			return GMWNBase->RegisterPlayer(this);
		}
	}

	return false;
}

void AWhiteNoiseCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ADoorSwingSingle* Door = Cast<ADoorSwingSingle>(OtherActor);

	if (Door)
	{
		Door->Open(this->GetActorLocation() - Door->GetActorLocation());
	}
}

void AWhiteNoiseCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	
	this->HandleEnemyLock();
	
	if (this->bIsFiring)
	{
		this->WeaponFire_Execute();
	}
}

void AWhiteNoiseCharacter::HandleMovement(FVector worldDirection, float fRate)
{
	this->AddMovementInput(worldDirection, fRate);
}

void AWhiteNoiseCharacter::HandleRotation(FVector worldPosition)
{
	FVector vecDirection = this->GetActorLocation() - worldPosition;
	vecDirection = FVector(vecDirection.X, vecDirection.Y, 0);
	vecDirection *= -1; //todo: fix calc

	/* currently only face upper body mesh */
	FRotator TargetRot = FRotationMatrix::MakeFromX(vecDirection).Rotator();
	this->UpperBodyRotation = TargetRot;

	//todo: handle lower torso rotation, set sidewalk & backwards animations
	//currently lower torso is facing walk direction, upper body is facing look direction,
	//results in weird spine rotation
}

void AWhiteNoiseCharacter::CameraFreeMove(FVector worldDirection, float fRate)
{
	FVector RelLoc = this->TopDownCameraComponent->GetRelativeTransform().GetLocation();
	RelLoc += worldDirection * fRate;

	if (RelLoc.Z >= this->MaxFreeCamDistance || RelLoc.Z <= (this->MaxFreeCamDistance * -1))
	{
		RelLoc.Z = this->TopDownCameraComponent->GetRelativeTransform().GetLocation().Z;
	}

	if (RelLoc.Y >= this->MaxFreeCamDistance || RelLoc.Y <= (this->MaxFreeCamDistance * -1))
	{
		RelLoc.Y = this->TopDownCameraComponent->GetRelativeTransform().GetLocation().Y;
	}

	this->TopDownCameraComponent->SetRelativeLocation(RelLoc);
}

void AWhiteNoiseCharacter::ResetFreeCam()
{
	this->TopDownCameraComponent->SetRelativeLocation(FVector(0, 0, 0));
}

bool AWhiteNoiseCharacter::CameraFreeMoveToggle(const bool bIsCameraFreeMovement)
{
	/* if Cutscene (e.g.) return false; */
	this->bIsCameraFreeMovement = bIsCameraFreeMovement;

	if (!this->bIsCameraFreeMovement)
	{
		this->ResetFreeCam();
	}

	return this->bIsCameraFreeMovement;
}

FVector AWhiteNoiseCharacter::GetMouseCursorPosition() const
{
	//todo: -replace GetWord()->GetFirstPlayerController() with a Multiplayer compatible solution (Local Splitscreen / Serverside), if needed

	FHitResult Hit;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		return Hit.ImpactPoint;
	}

	return{ 0, 0, 0 }; //todo: Add failed return to avoid wrong reaction
}

void AWhiteNoiseCharacter::ManualRotation(const FVector worldPosition)
{
	if (!this->bHasLockedEnemy)
	{
		this->HandleRotation(worldPosition);
	}
}

void AWhiteNoiseCharacter::HandleEnemyLock()
{
	if (this->bHasLockedEnemy && this->LockedEnemy != nullptr)
	{
		if (this->LockedEnemy->IsValidLowLevel())
		{
			if (this->LockedEnemy->IsAlive())
			{
				this->HandleRotation(this->LockedEnemy->GetActorLocation());
			}
			else
			{
				this->LockedEnemy = nullptr;
				this->bHasLockedEnemy = false;
			}
		}
	}
}

bool AWhiteNoiseCharacter::LockEnemy(ANPC* LockedEnemy)
{
	if (LockedEnemy != nullptr && LockedEnemy->IsValidLowLevel())
	{
		this->bHasLockedEnemy = true;
		this->LockedEnemy = LockedEnemy;
	}
	else
	{
		this->bHasLockedEnemy = false;
		this->LockedEnemy = nullptr;
	}

	return this->bHasLockedEnemy;
}

void AWhiteNoiseCharacter::Input_Pickup()
{
	//todo: Add a check, if mouse cursor points on a weapon. Pick up first.

	for (TActorIterator<AWeapon> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AWeapon* WeaponActor = *ActorItr;
		if (WeaponActor != nullptr && WeaponActor->IsValidLowLevel())
		{
			if (WeaponActor->GetState() == EWeaponState::WS_WORLD)
			{
				float fDistance = FVector::Dist(this->GetActorLocation(), WeaponActor->GetActorLocation());
				if (fDistance <= this->WeaponPickupDistance)
				{
					this->WeaponPickup(WeaponActor);
					return;
				}
			}
		}
	}
}

void AWhiteNoiseCharacter::ForceWeaponEquip(AWeapon* weaponActor)
{
	if (weaponActor != nullptr && weaponActor->IsValidLowLevel())
	{
		this->WeaponPickup(weaponActor);
	}
}

void AWhiteNoiseCharacter::WeaponPickup(AWeapon* weaponActor)
{
	if (this->CurrentWeapon != nullptr)
	{
		this->WeaponDrop();
	}
	
	weaponActor->ChangeState(EWeaponState::WS_EQUIP);
	this->CurrentWeapon = weaponActor;
	weaponActor->AttachToComponent(this->WeaponGripPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	this->CurrentAnimation = weaponActor->GetTargetAnimation();
}

void AWhiteNoiseCharacter::WeaponDrop()
{
	if (this->CurrentWeapon != nullptr && this->CurrentWeapon->IsValidLowLevel())
	{
		this->CurrentWeapon->DetachRootComponentFromParent(true);

		FVector newLocation = this->WeaponThrowArm->GetSocketLocation(USpringArmComponent::SocketName);
		this->CurrentWeapon->SetActorLocation(newLocation);
		this->CurrentWeapon->ChangeState(EWeaponState::WS_WORLD);
		
		this->CurrentWeapon = nullptr;
		this->CurrentAnimation = ETargetAnimation::TA_NORMAL;
	}
}

void AWhiteNoiseCharacter::WeaponThrow()
{
	if (this->CurrentWeapon != nullptr && this->CurrentWeapon->IsValidLowLevel() && !this->bIsCameraFreeMovement)
	{
		AWeapon* weaponActor = this->CurrentWeapon;
		this->WeaponDrop();

		const float throwForce = 100000.0f; //todo: calculate the force
		weaponActor->ChangeState(EWeaponState::WS_THROWN);
		weaponActor->WeaponMesh->AddImpulseAtLocation((this->UpperBodyRotation.Vector() * throwForce), weaponActor->GetActorLocation());
	}
}

void AWhiteNoiseCharacter::WeaponFire_Execute()
{
	if (this->CurrentWeapon != nullptr && this->CurrentWeapon->IsValidLowLevel() && !this->bIsCameraFreeMovement)
	{
		if (this->LockedEnemy == nullptr)
		{
			this->CurrentWeapon->Fire(this->GetMouseCursorPosition());
		}
		else
		{
			if (this->LockedEnemy->IsValidLowLevel())
			{
				this->CurrentWeapon->Fire(this->LockedEnemy->GetActorLocation());
			}
		}
	}
}

void AWhiteNoiseCharacter::WeaponFire_Start()
{
	if (this->CurrentWeapon != nullptr && this->CurrentWeapon->IsValidLowLevel() && !this->bIsCameraFreeMovement)
	{
		if (this->CurrentWeapon->GetWeaponType() == EWeaponType::WT_SINGLE)
		{
			this->WeaponFire_Execute();
		}
		else if (this->CurrentWeapon->GetWeaponType() == EWeaponType::WT_AUTOMATIC)
		{
			this->bIsFiring = true;
		}
	}
}

void AWhiteNoiseCharacter::WeaponFire_Stop()
{
	this->bIsFiring = false;
}

/** */
bool AWhiteNoiseCharacter::hasItemEquipped() const
{
	if (this->CurrentWeapon != nullptr)
	{
		return true;
	}

	return false;
}

/** */
FString AWhiteNoiseCharacter::GetItemName() const
{
	return this->CurrentWeapon->GetName();
}

/** */
UClass* AWhiteNoiseCharacter::GetItemClass() const
{
	return this->CurrentWeapon->GetClass();
}

/** */
FTransform AWhiteNoiseCharacter::GetItemTransform() const
{
	return this->CurrentWeapon->GetActorTransform();
}