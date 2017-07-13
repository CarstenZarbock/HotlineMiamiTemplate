// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once
#include "GameFramework/PlayerController.h"
#include "WNCharacter.h"
#include "WNPlayerController.generated.h"

UCLASS()
class AWhiteNoisePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWhiteNoisePlayerController();

protected:
	AWhiteNoiseCharacter* PlayerPawn;
	bool bCameraFreeMovement;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void SetPawn(APawn* InPawn) override;
	// End PlayerController interface

	void MoveForward(float fRate);
	void MoveSideward(float fRate);
	FVector GetMouseCursorPosition();
	void RotatePlayerToMouse();

	void PickupObject();
	void Fire();
	void Fire_Stop();
	void Throw();
	void CameraToggle();
	void StopCameraToggle();

	void CameraFreeMoveHandle(float DeltaSeconds);

	void LockEnemy();

	void RestartStage();
};


