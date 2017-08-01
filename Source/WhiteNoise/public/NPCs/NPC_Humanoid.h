// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "WNNPC.h"
#include "WNWeapon.h"
#include "NPC_Humanoid.generated.h"

/** */
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
	/* Character meshes / limb meshes */
	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshTorso;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshArmLeft;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshArmRight;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshHead;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshLegLeft;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* MeshLegRight;


	/* Gore particles for each limb */
	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* ArmLeftGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* ArmRightGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* HeadGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* LegRightGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* LegLeftGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* TorsoHeadGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* TorsoBottomGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* TorsoArmLeftGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* TorsoArmRightGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* BottomTorsoGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* BottomLeftLegGore;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* BottomRightLegGore;


	/** Socket to attach equipped item */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
		class USceneComponent* WeaponGripPoint;

	/** Current animation type (nothing / pistol / rifle / ...) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		ETargetAnimation ECurrentAnimation;

	/** Weapon type to spawn with */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "AI")
		TSubclassOf<AWeapon> AStarterWeapon;

private:
	/** Reference to current equipped item actor */
	AWeapon* ACurrentWeapon;

	/** */
	void SpawnStartWeapon();

	/** Activates the gore particles of given limb */
	void ActivateGoreParticles(ECharacterLimbs ECutLimb);

	/** Cuts all limbs, activates all gore particles */
	void ExecuteDeathFullsplat(FVector vecDirection);

	/** Cuts random limb, plays death animation */
	void ExecuteDeathAnimation(FVector vecDirection);

	/** seperates given limb mesh from master mesh */
	void CutLimb(USkeletalMeshComponent* LimbMesh, const bool bHardCut);

public:
	/** */
	AHumanoid();
	
	/** */
	virtual void BeginPlay() override;

	/** Why is this public / required? Check & replace */
	AWeapon* GetWeapon() { return ACurrentWeapon; }

	/** Why is this public / required? Check & replace */
	AWeapon* SetWeapon(AWeapon* ANewWeaponObject) { ACurrentWeapon = ANewWeaponObject; return ACurrentWeapon; }

	/** Why is this public? Check & replace */
	bool Weapon_Pickup(AWeapon* TargetWeaponActor);

	/** Why is this public? Check & replace */
	void Weapon_Drop();

	/** Executes death / AI mode change based on damage */
	virtual void DamageApply(int iDamageAmount, FVector vecDirection) override;

	/** AI cycle, replace with blackbord */
	virtual void HandleAI() override;

	/** AI crawling around, pathfinding, ... */
	void HandleCrawl();
};
