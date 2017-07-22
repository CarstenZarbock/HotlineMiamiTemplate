// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "WNTrigger.h"
#include "WNCharacter.h"

AWNTrigger::AWNTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject <USceneComponent>(TEXT("ROOT"));
	this->SetRootComponent(RootScene);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->bGenerateOverlapEvents = true;
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWNTrigger::OnOverlapBegin);
	//UE_LOG(LogTemp, Warning, TEXT("Trigger::Const"));
}

void AWNTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWNTrigger::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AWNTrigger::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("AWNTrigger::BeginOverlap"));
	if (this->bIsActive && Cast<AWhiteNoiseCharacter>(OtherActor) != nullptr)
	{
		this->TriggerExecute(OtherActor);
	}
}

void AWNTrigger::TriggerExecute(AActor* OtherActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("AWNTrigger::Execute"));
}

void AWNTrigger::SetActive(const bool bIsActive)
{

}