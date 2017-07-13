// Copyright 2016 Carsten Zarbock / Rebound-Software

#include "WhiteNoise.h"
#include "Door_SwingSingle.h"
#include "Engine.h"

ADoorSwingSingle::ADoorSwingSingle()
{
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);

	NoCollisionDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NoCollisionDoorMesh"));
	NoCollisionDoorMesh->SetupAttachment(DoorMesh);

	PhysConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysConstraint"));
	PhysConstraint->SetupAttachment(DoorMesh);

	PhysConstraint->SetConstrainedComponents(this->DoorMesh, "None", nullptr, "None");

	this->DoorMesh->OnComponentHit.AddDynamic(this, &ADoorSwingSingle::OnHit);
}

void ADoorSwingSingle::BeginPlay()
{
	Super::BeginPlay();
	this->Close();
}

void ADoorSwingSingle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (this->bIsOpen)
	{
		if (this->PhysConstraint->GetCurrentSwing1() > -10.0f && this->PhysConstraint->GetCurrentSwing1() < 10.0f)
		{
			if (bReadyToClose)
			{
				this->Close();
			}
		}
		else
		{
			bReadyToClose = true;
		}
	}
}

void ADoorSwingSingle::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	/* Knockout */
}

void ADoorSwingSingle::Open(FVector worldDirection)
{
	if (!this->bIsOpen)
	{
		this->bIsOpen = true;
		this->PhysConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 160.0f);
		this->DoorMesh->AddImpulse(worldDirection * 100.0f);
		bReadyToClose = false;
	}
}

void ADoorSwingSingle::Close()
{
	this->bIsOpen = false;
	this->PhysConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
}
