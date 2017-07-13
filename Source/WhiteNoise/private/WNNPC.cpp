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

	AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
	if (GMBase != nullptr)
	{
		AWhiteNoiseGameMode* GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
		if (GMWNBase != nullptr)
		{
			GMWNBase->Register(this, this->bIsGarbage);
		}
	}

	/* Set Stats */
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

const bool ANPC::PlayerInSight()
{
	//todo: Multiplayer
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
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "[AI ERROR] PLAYER CHARACTER INVALID");
	}

	return false;
}

void ANPC::HandleAI()
{

}

void ANPC::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "AEnemy");
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::FromInt(NormalImpulse.Size()));
}

void ANPC::ChangeState(ETargetEnemyState ENewState)
{
	switch (ENewState)
	{
	case ETargetEnemyState::TES_CRAWL:
		this->bIsCrawling = true;
		this->ChangeMovementState(EEnemyMovementState::EMS_CRAWL);
		break;
	case ETargetEnemyState::TES_DEAD:
		this->bIsAlive = false;

		AGameModeBase* GMBase = GetWorld()->GetAuthGameMode();
		if (GMBase != nullptr)
		{
			AWhiteNoiseGameMode* GMWNBase = Cast<AWhiteNoiseGameMode>(GMBase);
			if (GMWNBase != nullptr)
			{
				GMWNBase->RemoveAliveNPC(this);
			}
		}

		break;
	}
}

void ANPC::ChangeMovementState(EEnemyMovementState ENewState)
{
	switch (ENewState)
	{
	case EEnemyMovementState::EMS_WALK:
		this->GetCharacterMovement()->MaxWalkSpeed = this->fWalking;
		break;
	case EEnemyMovementState::EMS_RUN:
		this->GetCharacterMovement()->MaxWalkSpeed = this->fRunning;
		break;
	case EEnemyMovementState::EMS_CRAWL:
		this->GetCharacterMovement()->MaxWalkSpeed = this->fCrawling;
		break;
	default:
		break;
	}
}

FVector ANPC::GetRandomWalkpoint(bool inRange, float Range)
{
	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
	FNavLocation Location;

	if (NavSys != nullptr)
	{
		if (inRange)
		{
			NavSys->GetRandomReachablePointInRadius(this->GetActorLocation(), Range, Location);
		}
		else
		{
			NavSys->GetRandomPoint(Location, NavSys->MainNavData);
		}
	}

	return Location.Location;
}

bool ANPC::WalkToLocation(FVector vecDestinationLocation)
{
	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();

	if (NavSys != nullptr)
	{
		NavSys->SimpleMoveToLocation(this->GetController(), vecDestinationLocation);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::FromInt(vecDestinationLocation.X) + " " + FString::FromInt(vecDestinationLocation.Y) + " " + FString::FromInt(vecDestinationLocation.Z));
		return true;
	}

	return false;
}

FHitResult ANPC::TraceLine(FVector Start, FVector End, bool Debug)
{
	//todo: rework function
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = true;

	if (Debug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		RV_TraceParams.TraceTag = TraceTag;
	}

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,		       //result
		Start,			  //start
		End,			 //end
		ECC_Visibility, //collision channel
		RV_TraceParams
	);

	if (Debug)
	{
		DrawDebugLine(
			GetWorld(),
			Start,
			RV_Hit.ImpactPoint,
			FColor(255, 0, 0),
			false, -1, 0,
			12.333
		);
	}

	return RV_Hit;
}

void ANPC::DamageApply(int iDamageAmount, FVector vecDirection)
{

}