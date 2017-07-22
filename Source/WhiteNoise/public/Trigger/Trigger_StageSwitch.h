// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "WNTrigger.h"
#include "Trigger_StageSwitch.generated.h"

/**
 * 
 */
UCLASS()
class WHITENOISE_API AStageSwitch : public AWNTrigger
{
	GENERATED_BODY()
private:
	/** */
	virtual void TriggerExecute(AActor* OtherActor) override;

	/** */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FName LevelToLoad;

	/** */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FName PlayerStartName;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ArrowPointerMesh;

	/** */
	float TriggerCur = 2.0f;

public:
	/** */
	AStageSwitch();
	
	/** */
	virtual	void BeginPlay() override;

	/** */
	virtual void Tick(float DeltaTime) override;

	/** */
	virtual	void SetActive(const bool bIsActive) override;
};
