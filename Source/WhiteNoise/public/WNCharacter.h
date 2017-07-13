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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* WeaponThrowArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* WeaponGripPoint;


	/* Inventory */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WeaponPickupDistance = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	AWeapon* CurrentWeapon;

	bool bIsFiring;
	void WeaponFire_Execute();
	void WeaponPickup(AWeapon* weaponActor);
	
	/* Movement & Camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxFreeCamDistance = 2000.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	ETargetAnimation CurrentAnimation;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FRotator UpperBodyRotation;
	
	bool bIsCameraFreeMovement;

	void ResetFreeCam();

	void HandleRotation(FVector worldPosition);

	/* Actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actions", meta = (AllowPrivateAccess = "true"))
	ANPC* LockedEnemy;
	bool bHasLockedEnemy;

public:
	AWhiteNoiseCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	/* Movement & Camera */
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void HandleMovement(FVector worldDirection, float fRate);
	void CameraFreeMove(FVector worldDirection, float fRate);
	bool CameraFreeMoveToggle(const bool bIsCameraFreeMovement);
	FVector GetMouseCursorPosition() const;
	void ManualRotation(const FVector worldPosition);

	/* Inventory */
	void Input_Pickup();
	void WeaponThrow();
	void WeaponDrop();
	void WeaponFire_Start();
	void WeaponFire_Stop();

	/* Forces a weapon pickup, e.g. on World->BeginPlay() */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ForceWeaponEquip(AWeapon* weaponActor);

	/* Actions */
	bool LockEnemy(ANPC* LockedEnemy);
	void HandleEnemyLock();

	/* */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};

