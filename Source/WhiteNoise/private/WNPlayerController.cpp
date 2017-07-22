// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "WNPlayerController.h"
#include "WNNPC.h"
#include "WNGameMode.h"

AWhiteNoisePlayerController::AWhiteNoisePlayerController()
{
	bShowMouseCursor = false;
}

void AWhiteNoisePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	this->RotatePlayerToMouse();

	if (this->bCameraFreeMovement)
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
	InputComponent->BindAction("Restart", EInputEvent::IE_Released, this, &AWhiteNoisePlayerController::RestartStage);
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

	if (PlayerPawn != nullptr) //todo: replace with bPawnPossessed
	{
		if (fLocX <= fMoveX)
		{
			//**** left
			PlayerPawn->CameraFreeMove(FVector(0, 1, 0), fMoveSpeed * -1);
		}
		else if (fLocX >= ViewportSize.X - fMoveX)
		{
			//**** right
			PlayerPawn->CameraFreeMove(FVector(0, 1, 0), fMoveSpeed);
		}

		if (fLocY <= fMoveY)
		{
			//**** up
			PlayerPawn->CameraFreeMove(FVector(0, 0, 1), fMoveSpeed);
		}
		else if (fLocY >= ViewportSize.Y - fMoveY)
		{
			//**** down
			PlayerPawn->CameraFreeMove(FVector(0, 0, 1), fMoveSpeed * -1);
		}
	}

}

void AWhiteNoisePlayerController::RotatePlayerToMouse()
{
	if (PlayerPawn != nullptr) //todo: replace with bPawnPossessed
	{
		PlayerPawn->ManualRotation(this->GetMouseCursorPosition());
	}
}

void AWhiteNoisePlayerController::MoveForward(float fRate)
{
	if (PlayerPawn != nullptr && !this->bCameraFreeMovement)  //todo: replace with bPawnPossessed
	{
		PlayerPawn->HandleMovement(FVector(1, 0, 0), fRate);
	}
}

void AWhiteNoisePlayerController::MoveSideward(float fRate)
{
	if (PlayerPawn != nullptr && !this->bCameraFreeMovement)  //todo: replace with bPawnPossessed
	{
		PlayerPawn->HandleMovement(FVector(0, 1, 0), fRate);
	}
}

FVector AWhiteNoisePlayerController::GetMouseCursorPosition()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		return Hit.ImpactPoint;
	}
	return{ 0, 0, 0 }; //todo: replace with fail return to avoid mistaken rotation
}

void AWhiteNoisePlayerController::PickupObject()
{
	if (PlayerPawn != nullptr && !this->bCameraFreeMovement)  //todo: replace with bPawnPossessed
	{
		PlayerPawn->Input_Pickup();
	}
}

void AWhiteNoisePlayerController::Fire()
{
	if (PlayerPawn != nullptr) //todo: replace with bPawnPossessed
	{
		PlayerPawn->WeaponFire_Start();
	}
}

void AWhiteNoisePlayerController::Fire_Stop()
{
	if (PlayerPawn != nullptr) //todo: replace with bPawnPossessed
	{
		PlayerPawn->WeaponFire_Stop();
	}
}

void AWhiteNoisePlayerController::Throw()
{
	if (PlayerPawn != nullptr) //todo: replace with bPawnPossessed
	{
		if (!this->bCameraFreeMovement)
		{
			PlayerPawn->WeaponThrow();
		}
	}
}

void AWhiteNoisePlayerController::LockEnemy()
{
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
	if (PlayerPawn != nullptr) //todo: replace with bPawnPossessed
	{
		this->bCameraFreeMovement = PlayerPawn->CameraFreeMoveToggle(true);
	}
}

void AWhiteNoisePlayerController::StopCameraToggle()
{
	if (PlayerPawn != nullptr) //todo: replace with bPawnPossessed
	{
		this->bCameraFreeMovement = PlayerPawn->CameraFreeMoveToggle(false);
	}
}

void AWhiteNoisePlayerController::RestartStage()
{
	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	if (GMBase != nullptr)
	{
		AWhiteNoiseGameMode* GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
		if (GMWNBase != nullptr)
		{
			GMWNBase->RestartStage();
		}
	}
}