// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "Weapon.h"
#include "Enemy.h"
#include "WhiteNoiseCharacter.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	this->SetRootComponent(WeaponMesh);
	WeaponMesh->OnComponentHit.AddDynamic(this, &AWeapon::OnHit);

	WeaponShotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ShotPoint"));
	WeaponShotPoint->SetupAttachment(RootComponent);
	this->ChangeState(EWeaponState::WS_WORLD);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurAmmo = MaxAmmo;

	if (this->bIsPickupOnStart)
	{
		AWhiteNoiseCharacter* PlayerChar = Cast<AWhiteNoiseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerChar != nullptr && PlayerChar->IsValidLowLevel())
		{
			PlayerChar->WeaponPickup(this);
		}
	}
}

// Called every frame
void AWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	this->CustomTick(DeltaTime);

	

	if (this->GetState() == EWeaponState::WS_THROWN)
	{
		if (GetGameTimeSinceCreation() > fThrowTime + 0.5f)
		{
			if (this->GetVelocity().Size() < 50.0f)
			{
				this->ChangeState(EWeaponState::WS_WORLD);
			}
		}
	}

	if (this->bIsShotPause)
	{
		this->fShotPauseTimer += fShotrate * DeltaTime;
		if (this->fShotPauseTimer >= this->fShotPause)
		{
			this->bIsShotPause = false;
			this->fShotPauseTimer = 0;
		}
	}
}

void AWeapon::CustomTick(float DeltaTime)
{

}

void AWeapon::ChangeState(EWeaponState ENewState)
{
	FCollisionResponseContainer ColContainer;
	//0 None, 1 Overlap, 2 Block

	this->SetState(ENewState);
	switch (ENewState)
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
		this->fThrowTime = GetGameTimeSinceCreation();
		break;
	}
}

void AWeapon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<AEnemy>(OtherActor) != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "AWeapon");
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::FromInt(NormalImpulse.Size()));
	}
}

void AWeapon::Fire(FVector vecTargetLocation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, "Weapon::Fire");
}