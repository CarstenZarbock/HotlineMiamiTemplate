// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "WhiteNoisePlayerController.h"
#include "NPC.h"

AWhiteNoisePlayerController::AWhiteNoisePlayerController()
{
	bShowMouseCursor = false;
}

void AWhiteNoisePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	this->RotatePlayerToMouse();
	if (this->GetCameraFreeMovement())
	{
		this->CameraFreeMoveHandle(DeltaTime);
	}
}

void AWhiteNoisePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AWhiteNoisePlayerController::MoveForward);
	InputComponent->BindAxis("MoveSideward", this, &AWhiteNoisePlayerController::MoveSideward);
	InputComponent->BindAction("Pickup", EInputEvent::IE_Pressed, this, &AWhiteNoisePlayerController::PickupObject);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AWhiteNoisePlayerController::Fire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AWhiteNoisePlayerController::Fire_Stop);
	InputComponent->BindAction("Throw", EInputEvent::IE_Pressed, this, &AWhiteNoisePlayerController::Throw);
	InputComponent->BindAction("CameraToggle", EInputEvent::IE_Pressed, this, &AWhiteNoisePlayerController::CameraToggle);
	InputComponent->BindAction("CameraToggle", EInputEvent::IE_Released, this, &AWhiteNoisePlayerController::StopCameraToggle);
	InputComponent->BindAction("LockOn", EInputEvent::IE_Pressed, this, &AWhiteNoisePlayerController::LockEnemy);
}

void AWhiteNoisePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	PlayerPawn = Cast<AWhiteNoiseCharacter>(InPawn);
}

void AWhiteNoisePlayerController::CameraFreeMoveHandle(float DeltaSeconds)
{
	float fMovePercentage = 20.0f;
	float fMoveSpeed = 4000.0f * DeltaSeconds;
	
	float fLocX, fLocY;
	this->GetMousePosition(fLocX, fLocY);

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	float fMoveX = (ViewportSize.X / 100.0f) * fMovePercentage;
	float fMoveY = (ViewportSize.Y / 100.0f) * fMovePercentage;

	FString Message;
	Message.Append(FString::FromInt(fLocX));
	Message.Append(":");
	Message.Append(FString::FromInt(fMoveX));
	Message.Append(":");
	Message.Append(FString::FromInt(ViewportSize.X));

	GEngine->AddOnScreenDebugMessage(3, 10.0f, FColor::Cyan, Message);


	if (PlayerPawn != nullptr)
	{
		if (fLocX <= fMoveX)
		{
			//**** left
			const FVector vecWorldDirection = { 0, 1, 0 };
			PlayerPawn->CameraFreeMove(vecWorldDirection, fMoveSpeed * -1);
		}
		else if (fLocX >= ViewportSize.X - fMoveX)
		{
			//**** right
			const FVector vecWorldDirection = { 0, 1, 0 };
			PlayerPawn->CameraFreeMove(vecWorldDirection, fMoveSpeed);
		}

		if (fLocY <= fMoveY)
		{
			//**** up
			const FVector vecWorldDirection = { 0, 0, 1 };
			PlayerPawn->CameraFreeMove(vecWorldDirection, fMoveSpeed);
		}
		else if (fLocY >= ViewportSize.Y - fMoveY)
		{
			//**** down
			const FVector vecWorldDirection = { 0, 0, 1 };
			PlayerPawn->CameraFreeMove(vecWorldDirection, fMoveSpeed * -1);
		}
	}

}

void AWhiteNoisePlayerController::RotatePlayerToMouse()
{
	if (PlayerPawn != nullptr)
	{
		if (PlayerPawn->GetLockedEnemy() == nullptr)
		{
			PlayerPawn->HandleRotation(this->GetMouseCursorPosition());
		}
	}
}

void AWhiteNoisePlayerController::MoveForward(float fRate)
{
	/* Add Movement to the direction */
	if (PlayerPawn != nullptr && !this->GetCameraFreeMovement())
	{
		FVector vecWorldDirection = { 1, 0, 0 };
		PlayerPawn->HandleMovement(vecWorldDirection, fRate);
	}
}

void AWhiteNoisePlayerController::MoveSideward(float fRate)
{
	/* Add Movement to the direction */
	if (PlayerPawn != nullptr && !this->GetCameraFreeMovement())
	{
		FVector vecWorldDirection = { 0, 1, 0 };
		PlayerPawn->HandleMovement(vecWorldDirection, fRate);
	}
}

FVector AWhiteNoisePlayerController::GetMouseCursorPosition()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		return Hit.ImpactPoint;
	}
	return{ 0, 0, 0 };
}

void AWhiteNoisePlayerController::PickupObject()
{
	if (PlayerPawn != nullptr && !this->GetCameraFreeMovement())
	{
		PlayerPawn->Input_Pickup();
	}
}

void AWhiteNoisePlayerController::Fire()
{
	if (PlayerPawn != nullptr)
	{
		PlayerPawn->WeaponFire();
	}
}

void AWhiteNoisePlayerController::Fire_Stop()
{
	if (PlayerPawn != nullptr)
	{
		PlayerPawn->WeaponFire_Stop();
	}
}

void AWhiteNoisePlayerController::Throw()
{
	if (PlayerPawn != nullptr)
	{
		if (!this->GetCameraFreeMovement())
		{
			PlayerPawn->WeaponThrow();
		}
	}
}

void AWhiteNoisePlayerController::LockEnemy()
{
	//Get Enemy below Mouse Cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		ANPC* TargetEnemy = Cast<ANPC>(Hit.GetActor());
		if (TargetEnemy != nullptr && TargetEnemy->IsValidLowLevel())
		{
			PlayerPawn->LockEnemy(TargetEnemy);
			return;
		}
	}

	PlayerPawn->LockEnemy(nullptr);
}

void AWhiteNoisePlayerController::CameraToggle()
{
	this->SetCameraFreeMovement(true);
	if (PlayerPawn != nullptr)
	{
		PlayerPawn->SetCameraFreeMove(true);
	}
}

void AWhiteNoisePlayerController::StopCameraToggle()
{
	this->SetCameraFreeMovement(false);
	if (PlayerPawn != nullptr)
	{
		PlayerPawn->SetCameraFreeMove(false);
		PlayerPawn->ResetFreeCam();
	}
}