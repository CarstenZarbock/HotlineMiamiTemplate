// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h" //todo: Change to IWYU System
#include "WNWeapon.generated.h"

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
protected:
	EWeaponState EState;
	float fThrowTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		ETargetAnimation TargetAnimation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* WeaponShotPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float fAI_AttackRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float fShotrate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float fShotPause;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		uint8 MaxAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		uint8 CurAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (AllowPrivateAccess = "true"))
		uint8 SpawnStage;
	float fShotPauseTimer = 0;
	bool bIsShotPause = false;
	bool bIsGarbage;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		class USkeletalMeshComponent* WeaponMesh;

	// Sets default values for this actor's properties
	AWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	virtual void CustomTick(float DeltaSeconds);

	void MarkAsGarbage() { bIsGarbage = true; }

	void ChangeState(EWeaponState ENewState);
	EWeaponState GetState() const { return EState; }

	ETargetAnimation GetTargetAnimation() const { return TargetAnimation; }

	float GetAIAttackRange() const { return fAI_AttackRange; }

	EWeaponType GetWeaponType() const { return WeaponType; }
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Fire(FVector vecTargetLocation);
};
