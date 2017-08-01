// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "GameFramework/Actor.h"
#include "WNTrigger.generated.h"

UCLASS()
class WHITENOISE_API AWNTrigger : public AActor
{
	GENERATED_BODY()
private:
	/** Collision Box to toggle trigger */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionBox;

	/** */
	class USceneComponent* RootScene;

	/** Actions to execute on toggle */
	virtual void TriggerExecute(AActor* OtherActor);
	
protected:
	/** If not active, TriggerExecute won't be executed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		bool bIsActive;

	/** Used to get activated by last trigger */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FName TriggerTag;

public:	
	/** Checks if this is the actor last trigger is looking for */
	bool IsTrigger(const FName TriggerTag) const { if (this->TriggerTag == TriggerTag) { return true; } return false; };

	/** */
	AWNTrigger();

	/** */
	virtual void BeginPlay() override;
	
	/** */
	virtual void Tick( float DeltaSeconds ) override;

	/** */
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	/** (De-)Activates the trigger */
	virtual void SetActive(const bool bIsActive);
};
