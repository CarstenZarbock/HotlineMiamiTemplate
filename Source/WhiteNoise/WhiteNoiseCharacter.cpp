// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "WhiteNoise.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "WhiteNoiseCharacter.h"

AWhiteNoiseCharacter::AWhiteNoiseCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	WeaponThrowArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("WeaponThrowArm"));
	WeaponThrowArm->SetupAttachment(RootComponent);
	WeaponThrowArm->TargetArmLength = 150.0f;
	WeaponThrowArm->bDoCollisionTest = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
										  // Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	WeaponGripPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponGripPoint"));
	WeaponGripPoint->SetupAttachment(this->GetMesh());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	this->SetCurrentWeapon(nullptr);
	this->SetLockedEnemy(nullptr);

	this->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AWhiteNoiseCharacter::OnHit);

}

void AWhiteNoiseCharacter::BeginPlay()
{
	Super::BeginPlay();
	WeaponGripPoint->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightHand"));
	FVector bla = { -90.0f, 170.0f, 80.0f };
	WeaponGripPoint->SetRelativeRotation(FQuat::MakeFromEuler(bla));
	
}

void AWhiteNoiseCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	this->HandleEnemyLock();
	
	if (this->bIsShooting)
	{
		this->Weapon_Shot_Execute();
	}

	if (this->GetCurrentWeapon() != nullptr)
	{
		FString Message;
		Message.Append(FString::FromInt(this->GetCurrentWeapon()->CurAmmo));
		Message.Append("/");
		Message.Append(FString::FromInt(this->GetCurrentWeapon()->MaxAmmo));
		GEngine->AddOnScreenDebugMessage(10, 1.0f, FColor::Emerald, Message);
	}
}

void AWhiteNoiseCharacter::HandleEnemyLock()
{
	if (this->GetLockedEnemy() != nullptr)
	{
		if (this->GetLockedEnemy()->IsValidLowLevel())
		{
			if (this->GetLockedEnemy()->GetStats()->GetIsAlive())
			{
				this->HandleRotation(this->GetLockedEnemy()->GetActorLocation());
			}
			else
			{
				this->SetLockedEnemy(nullptr);
			}
		}
	}
}

void AWhiteNoiseCharacter::HandleMovement(FVector vecWorldDirection, float fRate)
{
	/* Add the actual movement input */
	this->AddMovementInput(vecWorldDirection, fRate);
}

void AWhiteNoiseCharacter::HandleRotation(FVector vecWorldPosition)
{
	FVector vecDirection = this->GetActorLocation() - vecWorldPosition;
	vecDirection = FVector(vecDirection.X, vecDirection.Y, 0);
	vecDirection *= -1;

	FRotator Rot = FRotationMatrix::MakeFromX(vecDirection).Rotator();
	//FRotator SocketRot = this->GetMesh()->GetSocketByName("Spine2")->RelativeRotation;
	
	UpperBodyRotation = Rot;
	//this->SetActorRotation(Rot);
}

void AWhiteNoiseCharacter::Input_Pickup()
{
	/* Add check for item below mouse cursor first */
	for (TActorIterator<AWeapon> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AWeapon* WeaponActor = *ActorItr;
		if (WeaponActor != nullptr && WeaponActor->IsValidLowLevel())
		{
			if (WeaponActor->GetState() == EWeaponState::WS_WORLD)
			{
				float fDistance = FVector::Dist(this->GetActorLocation(), WeaponActor->GetActorLocation());
				if (fDistance <= this->GetPickupDistance())
				{
					if (!this->Weapon_Pickup(WeaponActor))
					{
						/* Weapon couldn't be picked up, try again */
						this->Weapon_Pickup(WeaponActor);
					}

					return;
				}
			}
		}
	}
}

bool AWhiteNoiseCharacter::Weapon_Pickup(AWeapon* TargetWeaponActor)
{
	if (this->GetCurrentWeapon() == nullptr)
	{
		TargetWeaponActor->ChangeState(EWeaponState::WS_EQUIP);
		this->SetCurrentWeapon(TargetWeaponActor);
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

void AWhiteNoiseCharacter::Weapon_Drop()
{
	if (this->GetCurrentWeapon() != nullptr && this->GetCurrentWeapon()->IsValidLowLevel())
	{
		this->GetCurrentWeapon()->DetachRootComponentFromParent(true);
		FVector vecNewLocation = this->GetActorLocation() + (UpperBodyRotation.Vector() * 150);
		this->GetCurrentWeapon()->SetActorLocation(vecNewLocation);
		this->GetCurrentWeapon()->ChangeState(EWeaponState::WS_WORLD);
		this->SetCurrentWeapon(nullptr);
		this->ECurrentAnimation = ETargetAnimation::TA_NORMAL;
	}
}

void AWhiteNoiseCharacter::Weapon_Throw()
{
	if (this->GetCurrentWeapon() != nullptr && this->GetCurrentWeapon()->IsValidLowLevel() && !this->GetCameraFreeMove())
	{
		this->GetCurrentWeapon()->DetachRootComponentFromParent(true);
		//FVector vecNewLocation = this->GetActorLocation() + (UpperBodyRotation.Vector() * 150);
		FVector vecNewLocation = this->WeaponThrowArm->GetSocketLocation(USpringArmComponent::SocketName);

		this->GetCurrentWeapon()->SetActorLocation(vecNewLocation);
		this->GetCurrentWeapon()->ChangeState(EWeaponState::WS_THROWN);
		this->GetCurrentWeapon()->WeaponMesh->AddImpulseAtLocation((UpperBodyRotation.Vector() * 100000), this->GetCurrentWeapon()->GetActorLocation());
		this->SetCurrentWeapon(nullptr);
		this->ECurrentAnimation = ETargetAnimation::TA_NORMAL;
	}
}

void AWhiteNoiseCharacter::FreeMoveCameraUp(float fRate)
{
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, FString::FromInt(fRate));
	FVector RelLoc = this->TopDownCameraComponent->GetRelativeTransform().GetLocation();
	RelLoc.Z += fRate;
	if (RelLoc.Z < this->GetCameraFreeDistance() && RelLoc.Z > (this->GetCameraFreeDistance() * -1))
	{
		this->TopDownCameraComponent->SetRelativeLocation(RelLoc);
	}
}

void AWhiteNoiseCharacter::FreeMoveCameraSide(float fRate)
{
	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Orange, FString::FromInt(fRate));
	FVector RelLoc = this->TopDownCameraComponent->GetRelativeTransform().GetLocation();
	RelLoc.Y += fRate;

	if (RelLoc.Y < this->GetCameraFreeDistance() && RelLoc.Y > (this->GetCameraFreeDistance() * -1))
	{
		this->TopDownCameraComponent->SetRelativeLocation(RelLoc);
	}
}

void AWhiteNoiseCharacter::ResetFreeCam()
{
	this->TopDownCameraComponent->SetRelativeLocation(FVector(0, 0, 0));
}

void AWhiteNoiseCharacter::LockEnemy(AEnemy* AEnemyActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "Enemy Locked");
	this->SetLockedEnemy(AEnemyActor);
}

void AWhiteNoiseCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, "AWhiteNoiseCharacter");
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::FromInt(NormalImpulse.Size()));
}

void AWhiteNoiseCharacter::Weapon_Shot_Execute()
{
	if (this->GetCurrentWeapon() != nullptr && this->GetCurrentWeapon()->IsValidLowLevel() && !this->GetCameraFreeMove())
	{
		if (this->GetLockedEnemy() == nullptr)
		{
			this->GetCurrentWeapon()->Fire(this->GetMouseCursorPosition());
		}
		else
		{
			if (this->GetLockedEnemy()->IsValidLowLevel())
			{
				this->GetCurrentWeapon()->Fire(this->GetLockedEnemy()->GetActorLocation());
			}
		}
	}
}

void AWhiteNoiseCharacter::Weapon_Fire()
{
	if (this->GetCurrentWeapon() != nullptr && this->GetCurrentWeapon()->IsValidLowLevel() && !this->GetCameraFreeMove())
	{
		if (this->GetCurrentWeapon()->WeaponType == EWeaponType::WT_SINGLE)
		{
			this->Weapon_Shot_Execute();
		}
		else if (this->GetCurrentWeapon()->WeaponType == EWeaponType::WT_AUTOMATIC)
		{
			//set is shooting
			this->bIsShooting = true;
		}
	}
}

void AWhiteNoiseCharacter::Weapon_Fire_Stop()
{
	//set !shotting
	this->bIsShooting = false;
}

FVector AWhiteNoiseCharacter::GetMouseCursorPosition()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		return Hit.ImpactPoint;
	}
	return{ 0, 0, 0 };
}
