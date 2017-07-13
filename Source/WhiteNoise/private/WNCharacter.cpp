// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "WNCharacter.h"
#include "Door_SwingSingle.h"

/* ------------------------------------------
* AWhiteNoiseCharacter()
*/
AWhiteNoiseCharacter::AWhiteNoiseCharacter()
{
	/* Setup character capsule */
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AWhiteNoiseCharacter::OnHit);
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	/* disable input character rotation */
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/* setup the character movement */
	GetCharacterMovement()->bOrientRotationToMovement = false; //no character oritation to movement
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	/* setup components */
	/* spring arm component to determine the weapon drop / throw location */
	WeaponThrowArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("WeaponThrowArm"));
	WeaponThrowArm->SetupAttachment(RootComponent);
	WeaponThrowArm->TargetArmLength = 150.0f;
	WeaponThrowArm->bDoCollisionTest = true;

	/* Camera topdown spring arm component */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // no collision test, celing gets ignored

	/* top down camera */
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	/* socket component to attach the equipped weapon */
	WeaponGripPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponGripPoint"));
	WeaponGripPoint->SetupAttachment(this->GetMesh());

	/* general tick & init stuff */
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	this->CurrentWeapon = nullptr;
	this->LockedEnemy = nullptr;
}

/* ------------------------------------------
* Super::BeginPlay()
*/
void AWhiteNoiseCharacter::BeginPlay()
{
	Super::BeginPlay();
	/* mesh(es) not initialised in constructor, attach at beginplay */
	WeaponGripPoint->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightHand"));
	
	/* reset relative rotation after attachement */
	WeaponGripPoint->SetRelativeRotation(FQuat::MakeFromEuler(FVector(-90.0f, 170.0f, 80.0f))); //todo: Better solution?
}

/* ------------------------------------------
* OnHit()
* UFunction - Fired on physical Hit impact (e.g. Physical Weapon Hit, Door Hit)
* @Param UPrimitiveComponent* HitComponent
* @Param AActor* OtherActor
* @Param UPrimitiveComponent* OtherComponent
* @Param FVector NormalImpulse
* @Param HitResult
*/
void AWhiteNoiseCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	/* Implement functions for character knockout on physical hits (weapon throw impact, door hit impact, ...) */
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::FromInt(NormalImpulse.Size()));
	ADoorSwingSingle* Door = Cast<ADoorSwingSingle>(OtherActor);

	if (Door)
	{
		Door->Open(this->GetActorLocation() - Door->GetActorLocation());
	}
}

/* ------------------------------------------
* Tick()
* @Param float DeltaSeconds
*/
void AWhiteNoiseCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	
	this->HandleEnemyLock();
	
	if (this->bIsFiring)
	{
		this->WeaponFire_Execute();
	}
}
/* Movement & Camera */
/* ------------------------------------------
* HandleMovement()
* Handles character movement input
* @Param FVector worldDirection - world direction to move to
* @Param float fRate - Input Axis Rate amount to move
*/
void AWhiteNoiseCharacter::HandleMovement(FVector worldDirection, float fRate)
{
	/* Add the actual movement rate input in world direction */
	this->AddMovementInput(worldDirection, fRate);
}

/* ------------------------------------------
* HandleRotation()
* @Param FVector worldPosition - worldPosition to face to
* Rotates the character to face the given world position
*/
void AWhiteNoiseCharacter::HandleRotation(FVector worldPosition)
{
	/* calculate direction vector to face world position */
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

/* ------------------------------------------
* FreeMoveCamera()
* @Param FVector worldPosition - worldPosition to face to
* Moves the camera in free roam mode to the given world direction, based on the rate
*/
void AWhiteNoiseCharacter::CameraFreeMove(FVector worldDirection, float fRate)
{
	/* apply the move direction to the current relative location */
	FVector RelLoc = this->TopDownCameraComponent->GetRelativeTransform().GetLocation();
	RelLoc += worldDirection * fRate;

	/* Reset relative location if the camera location does reach the CameraDistance limit */
	if (RelLoc.Z >= this->MaxFreeCamDistance || RelLoc.Z <= (this->MaxFreeCamDistance * -1))
	{
		RelLoc.Z = this->TopDownCameraComponent->GetRelativeTransform().GetLocation().Z;
	}

	if (RelLoc.Y >= this->MaxFreeCamDistance || RelLoc.Y <= (this->MaxFreeCamDistance * -1))
	{
		RelLoc.Y = this->TopDownCameraComponent->GetRelativeTransform().GetLocation().Y;
	}

	/* set the new free roam camera location */
	this->TopDownCameraComponent->SetRelativeLocation(RelLoc);
}

/* ------------------------------------------
* ResetFreeCam()
* Resets the camera to the initial relative location if free roam mode has ended
*/
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

/* ------------------------------------------
* GetMouseCursorPosition()
* Returns projected world location below MouseCursor
*/
FVector AWhiteNoiseCharacter::GetMouseCursorPosition() const
{
	//todo: -probably replace TraceLine with other solution
	//todo: -replace GetWord()->GetFirstPlayerController() with a Multiplayer compatible solution (Local Splitscreen / Serverside), if needed

	/* Traceline to get the impact point position below mouse cursor */
	FHitResult Hit;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		return Hit.ImpactPoint;
	}

	/* no hit, return 0 */
	return{ 0, 0, 0 }; //todo: bad solution, character faces weird position if no collision occours
}

void AWhiteNoiseCharacter::ManualRotation(const FVector worldPosition)
{
	if (!this->bHasLockedEnemy)
	{
		this->HandleRotation(worldPosition);
	}
}

/* Actions */
/* ------------------------------------------
* HandleEnemyLock()
* Checks if an enemy is locked on, handles character rotation to face locked enemy
*/
void AWhiteNoiseCharacter::HandleEnemyLock()
{
	if (this->bHasLockedEnemy && this->LockedEnemy != nullptr)
	{
		if (this->LockedEnemy->IsValidLowLevel())
		{
			/* Currently locked on an enemy which is alive */
			if (this->LockedEnemy->IsAlive())
			{
				this->HandleRotation(this->LockedEnemy->GetActorLocation());
			}
			else
			{
				/* locked on a dead enemy, unlock */
				this->LockedEnemy = nullptr;
				this->bHasLockedEnemy = false;
			}
		}
	}
}

/* ------------------------------------------
* LockEnemy()
* @Param AEnemy* enemyActor - enemy actor to lock on
* Locks the character rotation / aiming to a given enemy actor
*/
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

/* Inventory */
/* ------------------------------------------
* Input_Pickup()
* Pickup Input executed by player controller
*/
void AWhiteNoiseCharacter::Input_Pickup()
{
	//todo: Add a check, if mouse cursor points on a weapon. Pick up first.

	/* iterate through weapon actors, pick up first one in pickup distance */
	for (TActorIterator<AWeapon> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AWeapon* WeaponActor = *ActorItr;
		if (WeaponActor != nullptr && WeaponActor->IsValidLowLevel())
		{
			/* only pickup weapon if state is in world, not eqipped */
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

/* ------------------------------------------
* WeaponPickup()
* @Param AWeapon* weaponActor - weapon actor to pickup
* Picks up a given weaponActor, drops equipped weapon first
*/
void AWhiteNoiseCharacter::WeaponPickup(AWeapon* weaponActor)
{
	if (this->CurrentWeapon != nullptr)
	{
		this->WeaponDrop();
	}
	
	/* change weapon actor state to eqipped, and pickup */
	weaponActor->ChangeState(EWeaponState::WS_EQUIP);
	this->CurrentWeapon = weaponActor;
	weaponActor->AttachToComponent(this->WeaponGripPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	/* Set player animation to weapon type animation (e.g. rifle, 2 handed, ...) */
	this->CurrentAnimation = weaponActor->GetTargetAnimation();
}

/* ------------------------------------------
* WeaponDrop()
* Drops the current equipped weapon
*/
void AWhiteNoiseCharacter::WeaponDrop()
{
	if (this->CurrentWeapon != nullptr && this->CurrentWeapon->IsValidLowLevel())
	{
		/* Detach weapon actor from character, place to throw / drop location, change weapon state to world */
		this->CurrentWeapon->DetachRootComponentFromParent(true);
		FVector newLocation = this->WeaponThrowArm->GetSocketLocation(USpringArmComponent::SocketName);
		this->CurrentWeapon->SetActorLocation(newLocation);
		this->CurrentWeapon->ChangeState(EWeaponState::WS_WORLD);
		
		/* no weapon equipped, set character animation back to normal */
		this->CurrentWeapon = nullptr;
		this->CurrentAnimation = ETargetAnimation::TA_NORMAL;
	}
}

/* ------------------------------------------
* WeaponThrow()
* Throws the current equipped weapon
*/
void AWhiteNoiseCharacter::WeaponThrow()
{
	if (this->CurrentWeapon != nullptr && this->CurrentWeapon->IsValidLowLevel() && !this->bIsCameraFreeMovement)
	{
		/* drop the weapon */
		AWeapon* weaponActor = this->CurrentWeapon;
		this->WeaponDrop();

		/* change state to thrown and apply force */
		const float throwForce = 100000.0f; //todo: calculate the force
		weaponActor->ChangeState(EWeaponState::WS_THROWN);
		weaponActor->WeaponMesh->AddImpulseAtLocation((this->UpperBodyRotation.Vector() * throwForce), weaponActor->GetActorLocation());
	}
}

/* ------------------------------------------
* WeaponFire_Execute()
* Executes the (Single) Fire Function of the current weapon, either free position or locked enemy position 
*/
void AWhiteNoiseCharacter::WeaponFire_Execute()
{
	if (this->CurrentWeapon != nullptr && this->CurrentWeapon->IsValidLowLevel() && !this->bIsCameraFreeMovement)
	{
		/* not locked on an enemy, use the cursor position as target position */
		if (this->LockedEnemy == nullptr)
		{
			this->CurrentWeapon->Fire(this->GetMouseCursorPosition());
		}
		else
		{
			/* locked on an enemy, use the enemy position as target position */
			if (this->LockedEnemy->IsValidLowLevel())
			{
				this->CurrentWeapon->Fire(this->LockedEnemy->GetActorLocation());
			}
		}
	}
}

/* ------------------------------------------
* WeaponFire()
* Handles the fire of a weapon, based on the weapon type. Enables automatic shooting on automatic weapon
*/
void AWhiteNoiseCharacter::WeaponFire_Start()
{
	if (this->CurrentWeapon != nullptr && this->CurrentWeapon->IsValidLowLevel() && !this->bIsCameraFreeMovement)
	{
		/* single shot weapon, execute one shot directly */
		if (this->CurrentWeapon->GetWeaponType() == EWeaponType::WT_SINGLE)
		{
			this->WeaponFire_Execute();
		}
		else if (this->CurrentWeapon->GetWeaponType() == EWeaponType::WT_AUTOMATIC)
		{
			/* automatic shooting weapon, enable to shoot over time (handled by tick) */
			this->bIsFiring = true;
		}
	}
}

/* ------------------------------------------
* WeaponFire_Stop()
* Stops automatic fire mode, executed on controller input "Fire" released
*/
void AWhiteNoiseCharacter::WeaponFire_Stop()
{
	this->bIsFiring = false;
}