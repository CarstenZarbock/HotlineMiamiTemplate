// Copyright 2016 Carsten Zarbock / Rebound-Software

#pragma once

#include "WNDoor.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "Door_SwingSingle.generated.h"

/**
 * 
 */
UCLASS()
class WHITENOISE_API ADoorSwingSingle : public AWNDoor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere)
		class UPhysicsConstraintComponent* PhysConstraint;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* NoCollisionDoorMesh;

	bool bReadyToClose;

public:
	ADoorSwingSingle();

	virtual void Open(FVector worldDirection) override;
	virtual void Close() override;

	virtual	void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

};
