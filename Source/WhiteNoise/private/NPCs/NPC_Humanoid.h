// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "NPC.h"
#include "Weapon.h"
#include "Humanoid.generated.h"

UENUM(BlueprintType)
enum class ECharacterLimbs : uint8
{
	LIMB_HEAD			UMETA(DisplayName = "HEAD"),
	LIMB_TORSO			UMETA(DisplayName = "TORSO"),
	LIMB_ARMLEFT		UMETA(DisplayName = "ARMLEFT"),
	LIMB_ARMRIGHT		UMETA(DisplayName = "ARMRIGHT"),
	LIMB_BOTTOM			UMETA(DisplayName = "BOTTOM"),
	LIMB_LEGLEFT		UMETA(DisplayName = "LEGLEFT"),
	LIMB_LEGRIGHT		UMETA(DisplayName = "LEGRIGHT")
};

/**
 * 
 */
UCLASS()
class WHITENOISE_API AHumanoid : public ANPC
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshTorso;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshArmLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshArmRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshHead;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshLegLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshLegRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* ArmLeftGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* ArmRightGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* HeadGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* LegRightGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* LegLeftGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* TorsoHeadGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* TorsoBottomGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* TorsoArmLeftGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* TorsoArmRightGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* BottomTorsoGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* BottomLeftLegGore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* BottomRightLegGore;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
		class USceneComponent* WeaponGripPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		ETargetAnimation ECurrentAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		TSubclassOf<AWeapon> AStarterWeapon;

private:
	AWeapon* ACurrentWeapon;
	void SpawnStartWeapon();
	void ActivateGoreParticles(ECharacterLimbs ECutLimb);
	void ExecuteDeathFullsplat(FVector vecDirection);
	void ExecuteDeathAnimation(FVector vecDirection);
	void CutLimb(USkeletalMeshComponent* LimbMesh);

public:

	AHumanoid();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AWeapon* GetWeapon() { return ACurrentWeapon; }
	AWeapon* SetWeapon(AWeapon* ANewWeaponObject) { ACurrentWeapon = ANewWeaponObject; return ACurrentWeapon; }

	bool Weapon_Pickup(AWeapon* TargetWeaponActor);
	void Weapon_Drop();

	virtual void DamageApply(int iDamageAmount, FVector vecDirection) override;
	virtual void HandleAI() override;
	void HandleCrawl();
};
