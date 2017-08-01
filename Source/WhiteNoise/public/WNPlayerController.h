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
	/** */
	AWhiteNoisePlayerController();

protected:
	/** Owned character pawn by controller */
	AWhiteNoiseCharacter* PlayerPawn;

	/** Camera state is in free roaming */
	bool bCameraFreeMovement;

	/** */
	virtual void PlayerTick(float DeltaTime) override;

	/** */
	virtual void SetupInputComponent() override;

	/** */
	virtual void SetPawn(APawn* InPawn) override;


	/** */
	void MoveForward(float fRate);

	/** */
	void MoveSideward(float fRate);

	/** Gets the world location of the mouse cursor, based on a Trace */
	FVector GetMouseCursorPosition();

	/** Rotates player character to mouse world location */
	void RotatePlayerToMouse();


	/** */
	void PickupObject();

	/** */
	void Fire();

	/** Executed if Fire Input Key is released */
	void Fire_Stop();

	/** Throws equipped item */
	void Throw();

	/** */
	void CameraToggle();

	/** */
	void StopCameraToggle();

	/** */
	void CameraFreeMoveHandle(float DeltaSeconds);

	/** Checks if an enemy is under mouse cursor and aim locks it */
	void LockEnemy();

	/** Calls restart stage on game mode */
	void RestartStage();
};


