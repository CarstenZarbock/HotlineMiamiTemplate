// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once

#include "GameFramework/Character.h"
#include "WNNPC.generated.h"

/* AI patroling types */
UENUM(BlueprintType)
enum class EEnemyMovement : uint8
{
	EM_IDLE			UMETA(DisplayName = "IDLE"),
	EM_RANDOMPATROL	UMETA(DisplayName = "RANDOMWALK"),
	EM_FIXEDPATROL	UMETA(DisplayName = "FIXEDWALK")
};

/* AI movement types */
UENUM(BlueprintType)
enum class EEnemyMovementState : uint8
{
	EMS_WALK		UMETA(DisplayName = "WALKING"),
	EMS_RUN			UMETA(DisplayName = "RUNNING"),
	EMS_CRAWL		UMETA(DisplayName = "CRAWLING")
};

/* AI patroling directon priority types */
UENUM(BlueprintType)
enum class EEnemyMovementPriority : uint8
{
	EMP_LEFT		UMETA(DisplayName = "LEFT"),
	EMP_RIGHT		UMETA(DisplayName = "RIGHT"),
	EMP_RANDOM		UMETA(DisplayName = "RANDOM")
};

/* AI state types on change */
UENUM(BlueprintType)
enum class ETargetEnemyState : uint8
{
	TES_CRAWL		UMETA(DisplayName = "CRAWLING"),
	TES_DEAD		UMETA(DisplayName = "DEAD")
};

UCLASS()
class WHITENOISE_API ANPC : public ACharacter
{
	GENERATED_BODY()
private:
	/* stats max health */
	UPROPERTY(EditAnywhere, Category = "Stats")
	int MaxHealth;
		
	/* */
	UPROPERTY(EditAnywhere, Category = "Stats")
	float fRunning;
	
	/* */
	UPROPERTY(EditAnywhere, Category = "Stats")
	float fWalking;
	
	/* */
	UPROPERTY(EditAnywhere, Category = "Stats")
	float fCrawling;	
	
	/* */
	bool bIsGarbage = false;
	
protected:
	/* Raycast wrapper, casts a traceline */
	FHitResult TraceLine(FVector Start, FVector End, bool Debug);

	/* */
	bool bIsMovingToPoint;

	/* */
	FVector vecTargetPoint;

	/* */
	bool bHasSeenPlayer;

	/* */
	bool bIsAlive;

	/* stats current health */
	int Health;

	/* */
	bool bIsCrawling;

public:
	/* Registers the object as a garbage object @Stage Handler*/
	void MarkAsGarbage() { bIsGarbage = true; }
	
	/* Returns the alive status */
	bool IsAlive() const { return this->bIsAlive; }

	/* Changes the NPC AI state */
	void ChangeState(ETargetEnemyState ENewState);
	
	/* AI Handling */
	virtual void HandleAI();
	
	/* Returns a random walkpoint on map */
	FVector GetRandomWalkpoint(bool inRange, float Range);
	
	/* Changes the NPC movement state */
	void ChangeMovementState(EEnemyMovementState ENewState);
	
	/* Activates movement to world position */
	bool WalkToLocation(FVector destinationWorldPosition);

	/* current movement state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyMovement EEnemyType;
	
	/* movement direction priority for random patroling*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemyMovementPriority EMovementPriority;
	
	/* different movement stats (/speeds) */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	//FMovementStats MovementStats;

	/* Death animation 1 - change to TArray*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UAnimationAsset* DeathAnimation_1;

	/* Constructor */
	ANPC();

	/* Begin Play */
	virtual void BeginPlay() override;
	
	/* Tick */
	virtual void Tick( float DeltaSeconds ) override;

	/* on physical hit */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	/* apply damage, reduce health, execute AI state change */
	virtual void DamageApply(int iDamageAmount, FVector vecDirection);

	/* checks if player is in vision */
	const bool PlayerInSight();

	/* BP wrapper for crawling getter - !remove! */
	UFUNCTION(BlueprintCallable, Category = "SunShine")
		bool BP_GetIsCrawling() const { return this->bIsCrawling; }
};
