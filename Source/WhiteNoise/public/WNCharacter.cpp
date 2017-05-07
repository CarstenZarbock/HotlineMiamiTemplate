// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "WhiteNoiseCharacter.h"

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
	this->SetCurrentWeapon(nullptr);
	this->SetLockedEnemy(nullptr);
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
}

/* ------------------------------------------
* Tick()
* @Param float DeltaSeconds
*/
void AWhiteNoiseCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	
	this->HandleEnemyLock();
	
	if (this->GetIsFiring())
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
	this->SetUpperBodyRotation(TargetRot);

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
	if (RelLoc.Z >= this->GetCameraFreeDistance() || RelLoc.Z <= (this->GetCameraFreeDistance() * -1))
	{
		RelLoc.Z = this->TopDownCameraComponent->GetRelativeTransform().GetLocation().Z;
	}

	if (RelLoc.Y >= this->GetCameraFreeDistance() || RelLoc.Y <= (this->GetCameraFreeDistance() * -1))
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


/* Actions */
/* ------------------------------------------
* HandleEnemyLock()
* Checks if an enemy is locked on, handles character rotation to face locked enemy
*/
void AWhiteNoiseCharacter::HandleEnemyLock()
{
	if (this->GetLockedEnemy() != nullptr)
	{
		if (this->GetLockedEnemy()->IsValidLowLevel())
		{
			/* Currently locked on an enemy which is alive */
			if (this->GetLockedEnemy()->GetStats()->GetIsAlive())
			{
				this->HandleRotation(this->GetLockedEnemy()->GetActorLocation());
			}
			else
			{
				/* locked on a dead enemy, unlock */
				this->SetLockedEnemy(nullptr);
			}
		}
	}
}

/* ------------------------------------------
* LockEnemy()
* @Param AEnemy* enemyActor - enemy actor to lock on
* Locks the character rotation / aiming to a given enemy actor
*/
void AWhiteNoiseCharacter::LockEnemy(ANPC* enemyActor)
{
	this->SetLockedEnemy(enemyActor);
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
				if (fDistance <= this->GetPickupDistance())
				{
					if (!this->WeaponPickup(WeaponActor))
					{
						//ULOG Something went wrong.
					}
					return;
				}
			}
		}
	}
}

/* ------------------------------------------
* WeaponPickup()
* @Param AWeapon* weaponActor - weapon actor to pickup
* Picks up a given weaponActor, drops equipped weapon first
*/
bool AWhiteNoiseCharacter::WeaponPickup(AWeapon* weaponActor)
{
	if (this->GetCurrentWeapon() != nullptr)
	{
		this->WeaponDrop();
	}

	if (this->GetCurrentWeapon() == nullptr) //better safe than sorry
	{
		/* change weapon actor state to eqipped, and pickup */
		weaponActor->ChangeState(EWeaponState::WS_EQUIP);
		this->SetCurrentWeapon(weaponActor);
		weaponActor->AttachToComponent(this->WeaponGripPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		/* Set player animation to weapon type animation (e.g. rifle, 2 handed, ...) */
		this->SetCurrentAnimation(weaponActor->GetTargetAnimation());
		return true;
	}

	return false;
}

/* ------------------------------------------
* WeaponDrop()
* Drops the current equipped weapon
*/
void AWhiteNoiseCharacter::WeaponDrop()
{
	if (this->GetCurrentWeapon() != nullptr && this->GetCurrentWeapon()->IsValidLowLevel())
	{
		/* Detach weapon actor from character, place to throw / drop location, change weapon state to world */
		this->GetCurrentWeapon()->DetachRootComponentFromParent(true);
		FVector newLocation = this->WeaponThrowArm->GetSocketLocation(USpringArmComponent::SocketName);
		this->GetCurrentWeapon()->SetActorLocation(newLocation);
		this->GetCurrentWeapon()->ChangeState(EWeaponState::WS_WORLD);
		
		/* no weapon equipped, set character animation back to normal */
		this->SetCurrentWeapon(nullptr);
		this->SetCurrentAnimation(ETargetAnimation::TA_NORMAL);
	}
}

/* ------------------------------------------
* WeaponThrow()
* Throws the current equipped weapon
*/
void AWhiteNoiseCharacter::WeaponThrow()
{
	if (this->GetCurrentWeapon() != nullptr && this->GetCurrentWeapon()->IsValidLowLevel() && !this->GetCameraFreeMove())
	{
		/* drop the weapon */
		AWeapon* weaponActor = this->GetCurrentWeapon();
		this->WeaponDrop();

		/* change state to thrown and apply force */
		const float throwForce = 100000.0f; //todo: calculate the force
		weaponActor->ChangeState(EWeaponState::WS_THROWN);
		weaponActor->WeaponMesh->AddImpulseAtLocation((this->GetUpperBodyRotation().Vector() * throwForce), weaponActor->GetActorLocation());
	}
}

/* ------------------------------------------
* WeaponFire_Execute()
* Executes the (Single) Fire Function of the current weapon, either free position or locked enemy position 
*/
void AWhiteNoiseCharacter::WeaponFire_Execute()
{
	if (this->GetCurrentWeapon() != nullptr && this->GetCurrentWeapon()->IsValidLowLevel() && !this->GetCameraFreeMove())
	{
		/* not locked on an enemy, use the cursor position as target position */
		if (this->GetLockedEnemy() == nullptr)
		{
			this->GetCurrentWeapon()->Fire(this->GetMouseCursorPosition());
		}
		else
		{
			/* locked on an enemy, use the enemy position as target position */
			if (this->GetLockedEnemy()->IsValidLowLevel())
			{
				this->GetCurrentWeapon()->Fire(this->GetLockedEnemy()->GetActorLocation());
			}
		}
	}
}

/* ------------------------------------------
* WeaponFire()
* Handles the fire of a weapon, based on the weapon type. Enables automatic shooting on automatic weapon
*/
void AWhiteNoiseCharacter::WeaponFire()
{
	if (this->GetCurrentWeapon() != nullptr && this->GetCurrentWeapon()->IsValidLowLevel() && !this->GetCameraFreeMove())
	{
		/* single shot weapon, execute one shot directly */
		if (this->GetCurrentWeapon()->WeaponType == EWeaponType::WT_SINGLE)
		{
			this->WeaponFire_Execute();
		}
		else if (this->GetCurrentWeapon()->WeaponType == EWeaponType::WT_AUTOMATIC)
		{
			/* automatic shooting weapon, enable to shoot over time (handled by tick) */
			this->SetIsFiring(true);
		}
	}
}

/* ------------------------------------------
* WeaponFire_Stop()
* Stops automatic fire mode, executed on controller input "Fire" released
*/
void AWhiteNoiseCharacter::WeaponFire_Stop()
{
	this->SetIsFiring(false);
}

ANPC* AWhiteNoiseCharacter::GetLockedEnemy() const
{
	return this->LockedEnemy;
}

void AWhiteNoiseCharacter::SetLockedEnemy(ANPC* LockedEnemy)
{
	this->LockedEnemy = LockedEnemy;
}

AWeapon* AWhiteNoiseCharacter::GetCurrentWeapon() const
{
	return this->CurrentWeapon; 
}

float AWhiteNoiseCharacter::GetPickupDistance() const
{
	return this->WeaponPickupDistance;
}

bool AWhiteNoiseCharacter::GetIsFiring() const
{
	return this->bIsFiring; 
}

void AWhiteNoiseCharacter::SetCurrentWeapon(const AWeapon* CurrentWeapon)
{
	this->CurrentWeapon = CurrentWeapon;
}

void AWhiteNoiseCharacter::SetPickupDistance(const float WeaponPickupDistance)
{
	this->WeaponPickupDistance = WeaponPickupDistance;
}

void AWhiteNoiseCharacter::SetIsFiring(const bool bIsFiring)
{
	this->bIsFiring = bIsFiring;
}

float AWhiteNoiseCharacter::GetCameraFreeDistance() const
{
	return MaxFreeCamDistance;
}

bool AWhiteNoiseCharacter::GetCameraFreeMove() const 
{
	return bIsCameraFreeMovement;
}

void AWhiteNoiseCharacter::SetCameraFreeDistance(const float MaxFreeCamDistance)
{
	this->MaxFreeCamDistance = MaxFreeCamDistance;
}

void AWhiteNoiseCharacter::SetCameraFreeMove(const bool bIsCameraFreeMovement)
{
	this->bIsCameraFreeMovement = bIsCameraFreeMovement;
}

ETargetAnimation AWhiteNoiseCharacter::GetCurrentAnimation() const
{
	return CurrentAnimation; 
}

void AWhiteNoiseCharacter::SetCurrentAnimation(const ETargetAnimation CurrentAnimation)
{
	this->CurrentAnimation = CurrentAnimation;
}

FRotator AWhiteNoiseCharacter::GetUpperBodyRotation() const 
{ 
	return UpperBodyRotation; 
}

void AWhiteNoiseCharacter::SetUpperBodyRotation(const FRotator UpperBodyRotation)
{
	this->UpperBodyRotation = UpperBodyRotation;
}
