// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "BulletTrace.h"
#include "Enemy.h"

ABulletTrace::ABulletTrace()
{
	FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("asdsdfsdf"));
	FireParticle->SetupAttachment(WeaponShotPoint);
}

void ABulletTrace::Fire(FVector vecTargetLocation)
{
	if (!this->bIsShotPause && this->CurAmmo > 0)
	{
		FVector vecTraceDirection = this->WeaponShotPoint->GetComponentLocation() - vecTargetLocation;
		FireParticle->SetWorldRotation(vecTraceDirection.ToOrientationQuat());
		FireParticle->Activate(true);
		vecTraceDirection = FVector(vecTraceDirection.X, vecTraceDirection.Y, 0);
		vecTraceDirection *= -1;

		for (int i = 0; i < iBulletAmount; i++)
		{
			float fRandom;
			if (i % 2 == 0)
			{
				fRandom = FMath::RandRange((fBulletSpread * i), (fBulletSpread * 2 * i));
			}
			else
			{
				fRandom = FMath::RandRange((fBulletSpread * i) * -1, (fBulletSpread * 2 * i) * -1);
			}

			vecTraceDirection = FVector(vecTraceDirection.X + fRandom, vecTraceDirection.Y + fRandom, 0);

			FHitResult Hit = this->TraceLine(this->WeaponShotPoint->GetComponentLocation(), this->WeaponShotPoint->GetComponentLocation() + (vecTraceDirection * fMaxRange), false);

			if (Hit.bBlockingHit)
			{
				if (Hit.GetActor() != nullptr && Hit.GetActor()->IsValidLowLevel())
				{
					AActor* HitActor = Hit.GetActor();
					AEnemy* EnemyActor = Cast<AEnemy>(HitActor);
					if (EnemyActor != nullptr)
					{
						FVector vecDirection = this->WeaponShotPoint->GetComponentLocation() - EnemyActor->GetActorLocation();
						float fDistance = FVector::Dist(this->GetActorLocation(), EnemyActor->GetActorLocation());
						int iDamage;
						if (fDistance > fCrawlDamageRange)
						{
							iDamage = 50;
						}
						else if (fDistance < fSplatDamageRange)
						{
							iDamage = 200;
						}
						else
						{
							iDamage = 100;
						}

						vecDirection = FVector(vecDirection.X, vecDirection.Y, 0);
						vecDirection *= -1;

						EnemyActor->DamageApply(iDamage, vecDirection * 100);

					}
				}
			}
		}

		bIsShotPause = true;
		this->CurAmmo -= 1;
	}
}

/* ------------------------------------------
* TraceLine
* @Param FVector Start - Start position world space
* @Param FVector End - End position world space
* @Param bool Debug - Draw Debug Lines
* Casts a trace line, returns hitresult
*/
FHitResult ABulletTrace::TraceLine(FVector Start, FVector End, bool Debug)
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


