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
	/** */
	ABulletTrace();

	/** Particle system used on firing the weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Particles")
		UParticleSystemComponent* FireParticle;

	/* Damage dealing system should be remade */
	/** Distance for instant death w/ fullsplat damage dealing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fSplatDamageRange;

	/** Distance for instant death w/ death animation damage dealing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fAnimDamageRange;

	/** Distance for crawling mode damage dealing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fCrawlDamageRange;

	/** Max distance for bullet to deal damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fMaxRange;

	/** Bullets calculated for each shot (e.g. Shotgun) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int iBulletAmount;

	/** aim spread for bullets (between -fBulletSpread & +fBulletspread)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float fBulletSpread;

	/** TraceLine helper function for Raycast */
	FHitResult TraceLine(FVector startWorldLocation, FVector endWorldLocation, bool bDebug);

	/** fire execute */
	void Fire(FVector targetWorldLocation) override;
};
