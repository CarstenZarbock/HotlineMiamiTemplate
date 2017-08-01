// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once
#include "GameFramework/Character.h"
#include "Engine.h" //todo: Change to IWYU System
#include "WNWeapon.h"
#include "WNNPC.h"
#include "WNCharacter.generated.h"

UCLASS(Blueprintable)
class AWhiteNoiseCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	/* Actor Components */
	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Spring arm to detect correct location for item throwing / dropping */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* WeaponThrowArm;

	/** Socket to attach the eqipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* WeaponGripPoint;


	/* Inventory */
	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WeaponPickupDistance = 500.0f;

	/** Actor reference to currently eqipped item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	AWeapon* CurrentWeapon;

	/** Required for auto weapons */
	bool bIsFiring;

	/** */
	void WeaponFire_Execute();

	/** */
	void WeaponPickup(AWeapon* weaponActor);
	
	/* Movement & Camera */
	/** Max camera distance, based on character location, for free roaming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxFreeCamDistance = 2000.0f;

	/** Current holding animation type (Normal, Pistol, Rifle, ...) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	ETargetAnimation CurrentAnimation;
	
	/** */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FRotator UpperBodyRotation;
	
	/** */
	bool bIsCameraFreeMovement;

	/** Sets the camera position back to character location */
	void ResetFreeCam();

	/** Rotates character to given world location */
	void HandleRotation(FVector worldPosition);

	/* Actions */
	/** Reference to aim locked enemy */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions", meta = (AllowPrivateAccess = "true"))
	ANPC* LockedEnemy;

	/** */
	bool bHasLockedEnemy;

public:
	/** */
	AWhiteNoiseCharacter();

	/** */
	virtual void Tick(float DeltaSeconds) override;

	/** */
	virtual void BeginPlay() override;

	/* Movement & Camera */
	/** */
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/** */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** */
	void HandleMovement(FVector worldDirection, float fRate);

	/** */
	void CameraFreeMove(FVector worldDirection, float fRate);

	/** */
	bool CameraFreeMoveToggle(const bool bIsCameraFreeMovement);

	/** Returns mouse cursor position in world location, based on a Trace */
	FVector GetMouseCursorPosition() const;

	/** Character Rotation based on mouse position, called by controller */
	void ManualRotation(const FVector worldPosition);

	/* Inventory */
	/** */
	void Input_Pickup();

	/** */
	void WeaponThrow();

	/** */
	void WeaponDrop();

	/** */
	void WeaponFire_Start();

	/** */
	void WeaponFire_Stop();

	/* Forces a weapon pickup, e.g. on World->BeginPlay() */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ForceWeaponEquip(AWeapon* weaponActor);

	/* Actions */
	/** Aimlocks an enemy */
	bool LockEnemy(ANPC* LockedEnemy);

	/** handles character rotation to locked enemy, and de-locking if dead */
	void HandleEnemyLock();

	/* */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};

