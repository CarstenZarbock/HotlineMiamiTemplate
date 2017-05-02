// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "Engine.h"
#include "Enemy.h"
#include "WhiteNoiseCharacter.generated.h"

UCLASS(Blueprintable)
class AWhiteNoiseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWhiteNoiseCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	class USceneComponent* WeaponGripPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fMaxFreeCamDistance = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fWeaponPickupDistance = 5000.0f;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* WeaponThrowArm;

	AWeapon* ACurrentWeapon;
	bool bCameraFreeMove;
	bool bIsShooting;

	AEnemy* ALockedEnemy;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		ETargetAnimation ECurrentAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		FRotator UpperBodyRotation;

	void HandleMovement(FVector vecWorldDirection, float fRate);
	void HandleRotation(FVector vecWorldPosition);

	AWeapon* SetCurrentWeapon(AWeapon* ANewWeapon) { ACurrentWeapon = ANewWeapon; return ACurrentWeapon; }

	UFUNCTION(BlueprintCallable, Category = "SunShine")
	AWeapon* GetCurrentWeapon() { return ACurrentWeapon; }

	float GetPickupDistance() { return fWeaponPickupDistance; }
	float SetPickupDistance(float fNewPickupDistance) { fWeaponPickupDistance = fNewPickupDistance; return fWeaponPickupDistance; }

	float GetCameraFreeDistance() {	return fMaxFreeCamDistance;	}
	float SetCameraFreeDistance(float fNewCameraDistance) {	fMaxFreeCamDistance = fNewCameraDistance; return fMaxFreeCamDistance; }

	AEnemy* SetLockedEnemy(AEnemy* ANewEnemy) { ALockedEnemy = ANewEnemy; return ALockedEnemy; }
	
	UFUNCTION(BlueprintCallable, Category = "SunShine")
	AEnemy* GetLockedEnemy() { return ALockedEnemy; }

	void Input_Pickup();
	bool Weapon_Pickup(AWeapon* TargetWeaponActor);
	void Weapon_Throw();
	void Weapon_Drop();
	bool SetCameraFreeMove(bool bNewCameraFreeMove) { bCameraFreeMove = bNewCameraFreeMove; return bCameraFreeMove; }
	bool GetCameraFreeMove() { return bCameraFreeMove; }
	void FreeMoveCameraUp(float fRate);
	void FreeMoveCameraSide(float fRate);
	void ResetFreeCam();
	void LockEnemy(AEnemy* AEnemyActor);
	void HandleEnemyLock();
	void Weapon_Fire();
	void Weapon_Fire_Stop();
	void Weapon_Shot_Execute();
	FVector GetMouseCursorPosition();

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

};

