// Copyright 2016 Carsten Zarbock / Rebound-Software
#pragma once

#include "WNWeapon.h"
#include "Weap_BTrace.generated.h"

/**
 * 
 */
UCLASS()
class WHITENOISE_API ABulletTrace : public AWeapon
{
	GENERATED_BODY()

public:
	ABulletTrace();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gore Particles")
		UParticleSystemComponent* FireParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fSplatDamageRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fAnimDamageRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fCrawlDamageRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fMaxRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iBulletAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fBulletSpread;

	FHitResult TraceLine(FVector Start, FVector End, bool Debug);
	void Fire(FVector vecTargetLocation) override;
};
