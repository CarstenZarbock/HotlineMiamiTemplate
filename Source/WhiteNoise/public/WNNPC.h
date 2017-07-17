// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once

#include "GameFramework/Character.h"
#include "WNNPC.generated.h"

/** AI patroling types */
UENUM(BlueprintType)
enum class EEnemyMovement : uint8
{
	EM_IDLE			UMETA(DisplayName = "IDLE"),
	EM_RANDOMPATROL	UMETA(DisplayName = "RANDOMWALK"),
	EM_FIXEDPATROL	UMETA(DisplayName = "FIXEDWALK")
};

/** AI movement types */
UENUM(BlueprintType)
enum class EEnemyMovementState : uint8
{
	EMS_WALK		UMETA(DisplayName = "WALKING"),
	EMS_RUN			UMETA(DisplayName = "RUNNING"),
	EMS_CRAWL		UMETA(DisplayName = "CRAWLING")
};

/** AI patroling directon priority types */
UENUM(BlueprintType)
enum class EEnemyMovementPriority : uint8
{
	EMP_LEFT		UMETA(DisplayName = "LEFT"),
	EMP_RIGHT		UMETA(DisplayName = "RIGHT"),
	EMP_RANDOM		UMETA(DisplayName = "RANDOM")
};

/** AI state types */
UENUM(BlueprintType)
enum class ETargetEnemyState : uint8
{
	TES_ALIVE		UMETA(DisplayName = "ALIVE"),
	TES_CRAWL		UMETA(DisplayName = "CRAWLING"),
	TES_DEAD		UMETA(DisplayName = "DEAD")
};

UCLASS()
class WHITENOISE_API ANPC : public ACharacter
{
	GENERATED_BODY()
private:
	/** Movement Run Speed */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float fRunning;
	
	/** Movement Walk Speed */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float fWalking;
	
	/** Movement Crawl Speed */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float fCrawling;	
	
	/** Register as garbage actor @StageHandler*/
	bool bIsGarbage = false;

	/** Register the actor on Game Mode / StageHandler */
	const bool RegisterOnGameMode();

	/** Notify Game Mode about death of the npc */
	const bool DeathNotifyGameMode();
	
protected:

	/** Raycast helper, casts a preset traceline */
	FHitResult TraceLine(FVector Start, FVector End, bool Debug);

	/** AI:: is actor currently moving */
	bool bIsMovingToLocation;

	/** AI:: target location of current movement */
	FVector TargetLocation;

	/** AI:: movement state - crawling on ground */
	bool bIsCrawling;

	/** AI:: has seen player at last check cycle */
	bool bHasSeenPlayer;

	/** Stats is NPC alive */
	bool bIsAlive;

	/** Stats current health */
	int Health;

	/** Stats maximum health */
	UPROPERTY(EditAnywhere, Category = "Stats")
	int MaxHealth;

public:
	/** Constructor */
	ANPC();

	/** Begin Play */
	virtual void BeginPlay() override;

	/** Tick */
	virtual void Tick(float DeltaSeconds) override;

	/** Called on physical hit */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	/** Current movement mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		EEnemyMovement EEnemyType;

	/* Movement direction priority for random patroling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		EEnemyMovementPriority EMovementPriority;

	/** Death animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		TArray<UAnimationAsset*> DeathAnimations;

	/** Registers the object as a garbage object @Stage Handler	*/
	void MarkAsGarbage() { this->bIsGarbage = true; }
	
	/** */
	bool IsAlive() const { return this->bIsAlive && (this->Health > 0); }

	/** Changes the current NPC AI state */
	void ChangeAIState(ETargetEnemyState ENewState);
	
	/** AI Cycle  - probably replace with Blackboard */
	virtual void HandleAI();
	
	/** AI:: Checks if the player is curently in sight */
	const bool PlayerInSight();

	/** Pathfinding - Returns a random walkpoint on map */
	FVector GetRandomWalkpoint(bool inRange, float Range);
	
	/** Changes the current NPC movement mode */
	void ChangeMovementState(EEnemyMovementState ENewState);
	
	/** Starts movement to a world location */
	bool WalkToLocation(FVector destinationWorldPosition);

	/** Applies damage to NPC, switches AI state */
	virtual void DamageApply(int iDamageAmount, FVector vecDirection);
};
