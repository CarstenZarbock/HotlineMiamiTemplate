// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once

#include "GameFramework/Character.h"
#include "Weapon.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovement : uint8
{
	EM_IDLE			UMETA(DisplayName = "IDLE"),
	EM_RANDOMPATROL	UMETA(DisplayName = "RANDOMWALK"),
	EM_FIXEDPATROL	UMETA(DisplayName = "FIXEDWALK")
};

UENUM(BlueprintType)
enum class EEnemyMovementState : uint8
{
	EMS_WALK		UMETA(DisplayName = "WALKING"),
	EMS_RUN			UMETA(DisplayName = "RUNNING"),
	EMS_CRAWL		UMETA(DisplayName = "CRAWLING")
};

UENUM(BlueprintType)
enum class EEnemyMovementPriority : uint8
{
	EMP_LEFT		UMETA(DisplayName = "LEFT"),
	EMP_RIGHT		UMETA(DisplayName = "RIGHT"),
	EMP_RANDOM		UMETA(DisplayName = "RANDOM")
};

UENUM(BlueprintType)
enum class ETargetEnemyState : uint8
{
	TES_CRAWL		UMETA(DisplayName = "CRAWLING"),
	TES_DEAD		UMETA(DisplayName = "DEAD")
};

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

USTRUCT(BlueprintType)
struct FStatx
{
	GENERATED_BODY()

private:
	unsigned int iHealth;
	bool bIsAlive;
	bool bIsCrawling;

public:
	FStatx() {}
	bool GetIsAlive() { return bIsAlive; }
	bool SetIsAlive(bool bNewIsAlive) { bIsAlive = bNewIsAlive; return bIsAlive; }
	bool GetIsCrawling() { return bIsCrawling; }
	bool SetIsCrawling(bool bNewIsCrawling) { bIsCrawling = bNewIsCrawling; return bIsCrawling; }
	unsigned int GetHealth() { return iHealth; }
	unsigned int SetHealh(unsigned int iNewHealth) { iHealth = iNewHealth; return iHealth; }
};

USTRUCT(BlueprintType)
struct FCrawlState
{
	GENERATED_BODY()
private:
	bool bIsMovingToPoint;
	FVector vecTargetPoint;

public:
	FCrawlState() {}
	bool GetIsMovingToPoint() { return bIsMovingToPoint; }
	bool SetIsMovingToPoint(bool bNewIsMoving) { bIsMovingToPoint = bNewIsMoving; return bIsMovingToPoint; }
	FVector GetTargetPoint() { return vecTargetPoint; }
	FVector SetTargetPoint(FVector vecNewTargetPoint) { vecTargetPoint = vecNewTargetPoint; return vecTargetPoint; }
};

USTRUCT(BlueprintType)
struct FAIState
{
	GENERATED_BODY()
private:
	bool bHasSeenPlayer;

public:

	bool GetHasSeenPlayer() { return bHasSeenPlayer; }
	bool SetHasSeenPlayer(bool bNewHasSeen) { bHasSeenPlayer = bNewHasSeen; return bHasSeenPlayer; }
	FAIState() {};
};

USTRUCT(BlueprintType)
struct FMovementStats
{
	GENERATED_BODY()
private:
public:
	FMovementStats() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float fRunning;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float fWalking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float fCrawling;

	float GetRunSpeed() { return fRunning; }
	float SetRunSpeed(float fNewRunSpeed) { fRunning = fNewRunSpeed; return fRunning; }
	float GetWalkSpeed() { return fWalking; }
	float SetWalkSpeed(float fNewWalkSpeed) { fWalking = fNewWalkSpeed; return fWalking; }
	float GetCrawlSpeed() { return fCrawling; }
	float SetCrawlSpeed(float fNewCrawlSpeed) { fCrawling = fNewCrawlSpeed; }
};

UCLASS()
class WHITENOISE_API AEnemy : public ACharacter
{
	GENERATED_BODY()
private:
	FStatx stats;
	FCrawlState CrawlState;
	AWeapon* ACurrentWeapon;
	FAIState AIState;

	FVector GetRandomWalkpoint(bool inRange, float Range);
	void ChangeState(ETargetEnemyState ENewState);
	void ChangeMovementState(EEnemyMovementState ENewState);
	void HandleCrawl();
	void HandleAITypes();
	bool WalkToLocation(FVector vecDestinationLocation);
	void ExecuteDeathFullsplat(FVector vecDirection);
	void ExecuteDeathAnimation(FVector vecDirection);
	void CutLimb(USkeletalMeshComponent* LimbMesh);
	void HandleAI();
	void ActivateGoreParticles(ECharacterLimbs ECutLimb);
	FHitResult TraceLine(FVector Start, FVector End, bool Debug);
	void SpawnStartWeapon();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		ETargetAnimation ECurrentAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		TSubclassOf<AWeapon> AStarterWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyMovement EEnemyType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyMovementPriority EMovementPriority;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FMovementStats MovementStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
		int iMaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
		class USceneComponent* WeaponGripPoint;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		UAnimationAsset* DeathAnimation_1;

	// Sets default values for this character's properties
	AEnemy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	FStatx* GetStats() { return &stats; }
	FCrawlState* GetCrawlState() { return &CrawlState; }
	FAIState* GetAIState() { return &AIState; }

	UFUNCTION(BlueprintCallable, Category = "SunShine")
		bool BP_GetIsCrawling() { return this->GetStats()->GetIsCrawling(); }

	void DamageApply(int iDamageAmount, FVector vecDirection);

	/* stats */
	int GetMaxHealth() { return iMaxHealth; }
	int SetMaxHealth(int iNewMaxHealth) { iMaxHealth = iNewMaxHealth; return iMaxHealth; }
	EEnemyMovement GetEnemyType() { return EEnemyType; }
	EEnemyMovement SetEnemyType(EEnemyMovement ENewEnemyType) { EEnemyType = ENewEnemyType; return EEnemyType; }
	EEnemyMovementPriority GetMovementPriority() { return EMovementPriority; }
	EEnemyMovementPriority SetMovementPriority(EEnemyMovementPriority ENewMovementPriority) { EMovementPriority = ENewMovementPriority; return EMovementPriority; }

	FMovementStats* GetMovement() { return &MovementStats; }

	AWeapon* GetWeapon() { return ACurrentWeapon; }
	AWeapon* SetWeapon(AWeapon* ANewWeaponObject) { ACurrentWeapon = ANewWeaponObject; return ACurrentWeapon; }

	bool Weapon_Pickup(AWeapon* TargetWeaponActor);
	void Weapon_Drop();

	bool PlayerInSight();
};
