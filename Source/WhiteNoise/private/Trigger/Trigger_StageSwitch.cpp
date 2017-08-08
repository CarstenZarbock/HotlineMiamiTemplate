// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "Trigger_StageSwitch.h"
#include "EngineUtils.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerStart.h"
#include "WNGameMode.h"
#include "WNCharacter.h"

AStageSwitch::AStageSwitch()
{
	ArrowPointerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowPointer"));
	ArrowPointerMesh->AttachTo(RootComponent);
}

void AStageSwitch::BeginPlay()
{
	Super::BeginPlay();
	this->SetActive(this->bIsActive);
}

void AStageSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->bIsActive)
	{
		//todo: try to replace this cast
		AWhiteNoiseCharacter* PlayerCharacter = Cast<AWhiteNoiseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerCharacter != nullptr)
		{
			FVector CamLocation = PlayerCharacter->GetTopDownCameraComponent()->GetComponentLocation();
			FVector ArrowLocation = this->ArrowPointerMesh->GetComponentLocation();

			FRotator newrot = (CamLocation - ArrowLocation).Rotation();
			newrot.Roll = 0.0f;
			newrot.Pitch -= 45.0f;
			newrot.Yaw = 90.0f;
			ArrowPointerMesh->SetWorldRotation(newrot);
		}

		FVector ArrowRelLocation = this->ArrowPointerMesh->GetRelativeTransform().GetLocation();

		if (ArrowRelLocation.X < 0.0f || ArrowRelLocation.X > 150.0f)
		{
			this->TriggerCur *= -1;
		}

		ArrowRelLocation.X += this->TriggerCur;

		this->ArrowPointerMesh->SetRelativeLocation(ArrowRelLocation);
	}
}

void AStageSwitch::SetActive(const bool bIsActive)
{
	this->bIsActive = bIsActive;
	this->ArrowPointerMesh->SetVisibility(this->bIsActive);
}

void AStageSwitch::TriggerExecute(AActor* OtherActor)
{
	//GameMode->Call Stage Switch
	this->SetActive(false);

	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	AWhiteNoiseGameMode* GMWNBase = nullptr;

	if (GMBase != nullptr)
	{
		GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
		if (GMWNBase != nullptr)
		{
			GMWNBase->IncreaseStage();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("StageSwitch::Execute %s"), *this->GetName());
	FLatentActionInfo LatentInfo;
	if (this->LevelToLoad != "None")
	{
		UGameplayStatics::LoadStreamLevel(this, this->LevelToLoad, true, true, LatentInfo);
		UE_LOG(LogTemp, Warning, TEXT("StageSwitch::LoadLevel %s"), *this->LevelToLoad.ToString());
	}

	for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APlayerStart* TargetStart = *ActorItr;
		if (TargetStart->PlayerStartTag == this->PlayerStartName)
		{
			OtherActor->SetActorLocation(TargetStart->GetActorLocation());
			OtherActor->SetActorRotation(TargetStart->GetActorRotation());
			UE_LOG(LogTemp, Warning, TEXT("StageSwitch::Teleport Player"));

			// -> Register Player
			if (GMWNBase != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("StageSwitch::Teleport Player 2"));
				GMWNBase->UpdatePlayer(Cast<APawn>(OtherActor));
			}
			return;
		}
	}
}