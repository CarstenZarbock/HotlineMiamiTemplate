// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h" //todo: IWYU
#include "WNWeapon.generated.h"

/** Weapon World States */
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	WS_WORLD		UMETA(DisplayName = "WORLD"),
	WS_EQUIP		UMETA(DisplayName = "EQUIPPED"),
	WS_THROWN		UMETA(DisplayName = "THROWN")
};

/** Character animation stance while eqipped */
UENUM(BlueprintType)
enum class ETargetAnimation : uint8
{
	TA_NORMAL			UMETA(DisplayName = "NORMAL"),
	TA_RIFLE			UMETA(DisplayName = "RIFLE")
};

/** Shot types, single / burst / auto */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_SINGLE			UMETA(DisplayName = "SINGLESHOT"),
	WT_BURST			UMETA(DisplayName = "BURST"),
	WT_AUTOMATIC		UMETA(DisplayName = "AUTOMATIC")
};

UCLASS()
class WHITENOISE_API AWeapon : public AActor
{
	GENERATED_BODY()

private:
	/** Register the actor on Game Mode / StageHandler */
	bool RegisterOnGameMode();

protected:
	/** Current weapon state - In World : Equipped : In Air */
	EWeaponState State;

	/** Time in ms since weapon has been thrown */
	float ThrowTime;

	/** Animation type characters are playing while holding the weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
		ETargetAnimation TargetAnimation;

	/** Location component used to spawn the projectile / fire the shot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* WeaponShotPoint;

	/** AI range to fire the weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
		float AIAttackRange;

	/** Singleshot / Burst / Automatic shots */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType;

	/** Pause time between each shot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
		float ShotPause;

	/** Current pause timer between shots */
	float fShotPauseTimer = 0;

	/** Time factor used to increment the current pause timer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
		float Shotrate;

	/** Break between shots */
	bool bIsShotPause = false;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
		uint8 MaxAmmo;

	/** Current ammo in clip */
	UPROPERTY(SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
		uint8 Ammo;

	/** Register as garbage actor @StageHandler*/
	bool bIsGarbage;

public:	
	/** */
	UPROPERTY(SaveGame)
	bool bIsRestartWeapon;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		class USkeletalMeshComponent* WeaponMesh;

	/** */
	AWeapon();

	/** */
	virtual void BeginPlay() override;
	
	/** */
	virtual void Tick( float DeltaSeconds ) override;

	/** Should be removed for virtual function performance reasons */
	virtual void CustomTick(float DeltaSeconds);

	/** */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	/** Registers the object as a garbage object @Stage Handler	*/
	void MarkAsGarbage() { this->bIsGarbage = true; }

	/** */
	bool UpdateAsGarbage();

	/** Executes firing */
	virtual void Fire(FVector targetWorldLocation);

	/** Changes the state of the weapon actor in the world */
	void ChangeState(EWeaponState newState);

	//Todo: Find a better solution for the following getter
	/** Returns the weapon state */
	EWeaponState GetState() const { return State; }

	/** Returns the animationtype used by characters while the weapon is equipped */
	ETargetAnimation GetTargetAnimation() const { return TargetAnimation; }

	/** Returns the min distance an AI has to reach to be able to attack */
	float GetAIAttackRange() const { return AIAttackRange; }

	/** Returns the weapon type to determine if autofire is possible */
	EWeaponType GetWeaponType() const { return WeaponType; }
};
