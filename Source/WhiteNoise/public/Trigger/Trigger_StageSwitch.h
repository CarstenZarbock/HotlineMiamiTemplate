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

	/** Target level to stream load, e.g. floor 1, floor 2, ... */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FName LevelToLoad;

	/** Tag / Name of Player Start Actor to teleport player to after level switch */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		FName PlayerStartName;

	/** Arrow mesh to point to trigger if trigger is activated */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ArrowPointerMesh;

	/** Current arrow position offset for up / down movement */
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
