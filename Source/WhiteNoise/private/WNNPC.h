// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once

#include "GameFramework/Character.h"
#include "NPC.generated.h"

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
class WHITENOISE_API ANPC : public ACharacter
{
	GENERATED_BODY()
private:
	FStatx stats;
	FCrawlState CrawlState;
	FAIState AIState;

public:
	void ChangeState(ETargetEnemyState ENewState);
	virtual void HandleAI();
	FVector GetRandomWalkpoint(bool inRange, float Range);
	void ChangeMovementState(EEnemyMovementState ENewState);
	bool WalkToLocation(FVector vecDestinationLocation);
	FHitResult TraceLine(FVector Start, FVector End, bool Debug);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyMovement EEnemyType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyMovementPriority EMovementPriority;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FMovementStats MovementStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
		int iMaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		UAnimationAsset* DeathAnimation_1;

	// Sets default values for this character's properties
	ANPC();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	FStatx* GetStats() { return &stats; }
	FCrawlState* GetCrawlState() { return &CrawlState; }
	FAIState* GetAIState() { return &AIState; }

	virtual void DamageApply(int iDamageAmount, FVector vecDirection);

	/* stats */
	int GetMaxHealth() { return iMaxHealth; }
	int SetMaxHealth(int iNewMaxHealth) { iMaxHealth = iNewMaxHealth; return iMaxHealth; }
	EEnemyMovement GetEnemyType() { return EEnemyType; }
	EEnemyMovement SetEnemyType(EEnemyMovement ENewEnemyType) { EEnemyType = ENewEnemyType; return EEnemyType; }
	EEnemyMovementPriority GetMovementPriority() { return EMovementPriority; }
	EEnemyMovementPriority SetMovementPriority(EEnemyMovementPriority ENewMovementPriority) { EMovementPriority = ENewMovementPriority; return EMovementPriority; }

	FMovementStats* GetMovement() { return &MovementStats; }

	bool PlayerInSight();

	UFUNCTION(BlueprintCallable, Category = "SunShine")
		bool BP_GetIsCrawling() { return this->GetStats()->GetIsCrawling(); }
};
