// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "WNWeapon.h"
#include "WNNPC.h"
#include "WNCharacter.h"
#include "WNGameMode.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	this->SetRootComponent(WeaponMesh);
	WeaponMesh->OnComponentHit.AddDynamic(this, &AWeapon::OnHit);

	WeaponShotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ShotPoint"));
	WeaponShotPoint->SetupAttachment(RootComponent);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	this->RegisterOnGameMode();

	//init stats - BeginPlay because SpawnActorDeferred@StageHandle
	if (!bIsRestartWeapon)
	{
		this->Ammo = this->MaxAmmo;
	}

	//Changing physics state causes weird location errors @SpawnActorDeferred
	//this->ChangeState(EWeaponState::WS_WORLD);
}

bool AWeapon::RegisterOnGameMode()
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

void AWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	this->CustomTick( DeltaTime );

	if (this->GetState() == EWeaponState::WS_THROWN)
	{
		//todo: Replace with DeltaTime solution
		if (GetGameTimeSinceCreation() > this->ThrowTime + 0.5f)
		{
			if (this->GetVelocity().Size() < 50.0f)
			{
				this->ChangeState(EWeaponState::WS_WORLD);
			}
		}
	}

	if (this->bIsShotPause)
	{
		this->fShotPauseTimer += this->Shotrate * DeltaTime;
		if (this->fShotPauseTimer >= this->ShotPause)
		{
			this->bIsShotPause = false;
			this->fShotPauseTimer = 0;
		}
	}
}

void AWeapon::CustomTick(float DeltaTime)
{

}

void AWeapon::ChangeState(EWeaponState newState)
{
	FCollisionResponseContainer ColContainer;

	//0 None, 1 Overlap, 2 Block
	this->State = newState;

	switch (this->State)
	{
	case EWeaponState::WS_WORLD:
		this->WeaponMesh->SetCustomDepthStencilValue(1);
		this->WeaponMesh->SetCollisionObjectType(ECC_WorldDynamic);
		this->WeaponMesh->SetSimulatePhysics(true);
		this->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		ColContainer.WorldStatic = 2;
		ColContainer.WorldDynamic = 1;
		ColContainer.Pawn = 1;

		this->WeaponMesh->SetCollisionResponseToChannels(ColContainer);
		break;

	case EWeaponState::WS_EQUIP:
		this->WeaponMesh->SetCustomDepthStencilValue(0);
		this->WeaponMesh->SetCollisionObjectType(ECC_WorldDynamic);
		this->WeaponMesh->SetSimulatePhysics(false);
		this->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ColContainer.WorldStatic = 0;
		ColContainer.WorldDynamic = 0;
		ColContainer.Pawn = 0;

		this->WeaponMesh->SetCollisionResponseToChannels(ColContainer);
		break;

	case EWeaponState::WS_THROWN:
		this->WeaponMesh->SetCustomDepthStencilValue(0);
		this->WeaponMesh->SetCollisionObjectType(ECC_WorldDynamic);
		this->WeaponMesh->SetSimulatePhysics(true);
		this->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		ColContainer.WorldStatic = 2;
		ColContainer.WorldDynamic = 2;
		ColContainer.Pawn = 2;

		this->WeaponMesh->SetCollisionResponseToChannels(ColContainer);
		this->ThrowTime = GetGameTimeSinceCreation();
		break;
	}
}

void AWeapon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<ANPC>(OtherActor) != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::FromInt(NormalImpulse.Size()));
	}
}

void AWeapon::Fire(FVector targetWorldLocation)
{
	
}