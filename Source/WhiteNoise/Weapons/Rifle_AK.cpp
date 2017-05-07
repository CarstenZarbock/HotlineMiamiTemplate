// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "Rifle_AK.h"

ARifle_AK::ARifle_AK()
{
	/* Default values, edit in BP class */
	fSplatDamageRange = 200.00f;
	fAnimDamageRange = 400.0f;
	fCrawlDamageRange = 1000.0f;
	fMaxRange = 10000.0f;
	iBulletAmount = 1;
	fBulletSpread = 0.0f;
	fShotrate = 10.0f;
	fShotPause = 1.0f;
	MaxAmmo = 30;
	WeaponType = EWeaponType::WT_AUTOMATIC;
	TargetAnimation = ETargetAnimation::TA_RIFLE;
	fAI_AttackRange = 2000.0f;
}

