// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "WhiteNoiseCharacter.h"
#include "WhiteNoisePlayerController.generated.h"

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

	bool SetCameraFreeMovement(bool bNewCameraFreeMovement) { bCameraFreeMovement = bNewCameraFreeMovement; return bCameraFreeMovement; }
	bool GetCameraFreeMovement() { return bCameraFreeMovement; }
	void CameraFreeMoveHandle(float DeltaSeconds);

	void LockEnemy();
};


