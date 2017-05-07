// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "NPC.h"
#include "WhiteNoiseCharacter.h"

// Sets default values
ANPC::ANPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//this->GetMesh()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
	this->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ANPC::OnHit);
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();

	/* Set Stats */
	this->GetStats()->SetIsAlive(true);
	this->GetStats()->SetHealh(this->GetMaxHealth());
	this->GetStats()->SetIsCrawling(false);
	
	this->ChangeMovementState(EEnemyMovementState::EMS_WALK);
}

// Called every frame
void ANPC::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	this->HandleAI();
}

bool ANPC::PlayerInSight()
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
		this->GetStats()->SetIsCrawling(true);
		this->ChangeMovementState(EEnemyMovementState::EMS_CRAWL);
		break;
	case ETargetEnemyState::TES_DEAD:
		this->GetStats()->SetIsAlive(false);
		break;
	}
}

void ANPC::ChangeMovementState(EEnemyMovementState ENewState)
{
	switch (ENewState)
	{
	case EEnemyMovementState::EMS_WALK:
		this->GetCharacterMovement()->MaxWalkSpeed = this->GetMovement()->GetWalkSpeed();
		break;
	case EEnemyMovementState::EMS_RUN:
		this->GetCharacterMovement()->MaxWalkSpeed = this->GetMovement()->GetRunSpeed();
		break;
	case EEnemyMovementState::EMS_CRAWL:
		this->GetCharacterMovement()->MaxWalkSpeed = this->GetMovement()->GetCrawlSpeed();
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
		return true;
	}

	return false;
}
/* ------------------------------------------
* TraceLine
* @Param FVector Start - Start position world space
* @Param FVector End - End position world space
* @Param bool Debug - Draw Debug Lines
* Casts a trace line, returns hitresult
*/
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