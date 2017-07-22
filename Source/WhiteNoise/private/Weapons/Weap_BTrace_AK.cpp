// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "Weap_BTrace_AK.h"

ARifle_AK::ARifle_AK()
{
	/* Default values, edit in BP class */
	fSplatDamageRange = 200.00f;
	fAnimDamageRange = 400.0f;
	fCrawlDamageRange = 1000.0f;
	fMaxRange = 10000.0f;
	iBulletAmount = 1;
	fBulletSpread = 0.0f;
	Shotrate = 10.0f;
	ShotPause = 1.0f;
	MaxAmmo = 30;
	WeaponType = EWeaponType::WT_AUTOMATIC;
	TargetAnimation = ETargetAnimation::TA_RIFLE;
	AIAttackRange = 2000.0f;
}

