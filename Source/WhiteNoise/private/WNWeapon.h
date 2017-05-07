// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h" //todo: Change to IWYU System
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	WS_WORLD		UMETA(DisplayName = "WORLD"),
	WS_EQUIP		UMETA(DisplayName = "EQUIPPED"),
	WS_THROWN		UMETA(DisplayName = "THROWN")
};

UENUM(BlueprintType)
enum class ETargetAnimation : uint8
{
	TA_NORMAL			UMETA(DisplayName = "NORMAL"),
	TA_RIFLE			UMETA(DisplayName = "RIFLE")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_SINGLE			UMETA(DisplayName = "SINGLESHOT"),
	WT_AUTOMATIC		UMETA(DisplayName = "AUTOMATIC")
};

UCLASS()
class WHITENOISE_API AWeapon : public AActor
{
	GENERATED_BODY()

private:
	EWeaponState EState;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		class USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		ETargetAnimation TargetAnimation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
		class USceneComponent* WeaponShotPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		bool bIsPickupOnStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		float fAI_AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float fShotrate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float fShotPause;

	float fShotPauseTimer = 0;
	bool bIsShotPause = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int CurAmmo;

	// Sets default values for this actor's properties
	AWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	virtual void CustomTick(float DeltaSeconds);

	void ChangeState(EWeaponState ENewState);
	EWeaponState SetState(EWeaponState ENewState) { EState = ENewState; return EState; }
	EWeaponState GetState() { return EState; }
	ETargetAnimation GetTargetAnimation() { return TargetAnimation; }
	float GetAIAttackRange() { return fAI_AttackRange; }

	float fThrowTime;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Fire(FVector vecTargetLocation);
};
