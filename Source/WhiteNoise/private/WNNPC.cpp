// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "WNNPC.h"
#include "WNCharacter.h"
#include "WNGameMode.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	this->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ANPC::OnHit);
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();

	this->RegisterOnGameMode();

	//init stats - Not in constructor because of SpawnActorDeferred
	this->bIsAlive = true;
	this->Health = this->MaxHealth;
	this->bIsCrawling = false;
	
	this->ChangeMovementState(EEnemyMovementState::EMS_WALK);
}

void ANPC::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	this->HandleAI();
}

const bool ANPC::RegisterOnGameMode()
{
	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	if (GMBase != nullptr)
	{
		AWhiteNoiseGameMode* GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
		if (GMWNBase != nullptr)
		{
			return GMWNBase->Register(this, this->bIsGarbage);
		}
	}

	return false;
}

const bool ANPC::DeathNotifyGameMode()
{
	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	if (GMBase != nullptr)
	{
		AWhiteNoiseGameMode* GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
		if (GMWNBase != nullptr)
		{
			return GMWNBase->RemoveAliveNPC(this);
		}
	}

	return false;
}

const bool ANPC::PlayerInSight()
{
	//todo: We probably want to avoid this cast each call
	AWhiteNoiseCharacter* PlayerCharacter = Cast<AWhiteNoiseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerCharacter != nullptr)
	{
		FHitResult Hit = TraceLine(this->GetActorLocation(), PlayerCharacter->GetActorLocation(), false);
		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor() == PlayerCharacter)
			{
				return true;
			}
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "[AI ERROR] PLAYER CHARACTER INVALID");
	}

	return false;
}

void ANPC::HandleAI()
{

}

void ANPC::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::FromInt(NormalImpulse.Size()));
}

void ANPC::ChangeAIState(ETargetEnemyState newState)
{
	switch (newState)
	{
	case ETargetEnemyState::TES_CRAWL:
		this->bIsCrawling = true;
		this->ChangeMovementState(EEnemyMovementState::EMS_CRAWL);
		
		break;

	case ETargetEnemyState::TES_DEAD:
		this->bIsAlive = false;
		this->DeathNotifyGameMode();

		break;
	}
}

void ANPC::ChangeMovementState(EEnemyMovementState newState)
{
	switch (newState)
	{
	case EEnemyMovementState::EMS_WALK:
		this->GetCharacterMovement()->MaxWalkSpeed = this->SpeedWalking;
		break;
	case EEnemyMovementState::EMS_RUN:
		this->GetCharacterMovement()->MaxWalkSpeed = this->SpeedRunning;
		break;
	case EEnemyMovementState::EMS_CRAWL:
		this->GetCharacterMovement()->MaxWalkSpeed = this->SpeedCrawling;
		break;
	default:
		break;
	}
}

FVector ANPC::GetRandomWalkpoint(bool bInRange = false, float range = 5000.0f)
{
	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
	FNavLocation WorldLocation;

	if (NavSys != nullptr)
	{
		if (bInRange)
		{
			NavSys->GetRandomReachablePointInRadius(this->GetActorLocation(), range, WorldLocation);
		}
		else
		{
			NavSys->GetRandomPoint(WorldLocation, NavSys->MainNavData);
		}
	}

	return WorldLocation.Location;
}

bool ANPC::WalkToLocation(FVector destinationWorldLocation)
{
	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();

	if (NavSys != nullptr)
	{
		NavSys->SimpleMoveToLocation(this->GetController(), destinationWorldLocation);
		return true;
	}

	return false;
}

void ANPC::DamageApply(int damageAmount, FVector worldDirection)
{

}

FHitResult ANPC::TraceLine(FVector startWorldLocation, FVector endWorldLocation, bool bDebug)
{
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = true;

	if (bDebug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		RV_TraceParams.TraceTag = TraceTag;
	}

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,							//result
		startWorldLocation,			  //start
		endWorldLocation,			 //end
		ECC_Visibility,				//collision channel
		RV_TraceParams
	);

	if (bDebug)
	{
		DrawDebugLine(
			GetWorld(),
			startWorldLocation,
			RV_Hit.ImpactPoint,
			FColor(255, 0, 0),
			false, -1, 0,
			12.333
		);
	}

	return RV_Hit;
}