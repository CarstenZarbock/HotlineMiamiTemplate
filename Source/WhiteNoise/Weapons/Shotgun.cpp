// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "Shotgun.h"

AShotgun::AShotgun()
{
	/* Default values, edit in BP class */
	fSplatDamageRange = 500.00f;
	fAnimDamageRange = 700.0f;
	fCrawlDamageRange = 1000.0f;
	fMaxRange = 10000.0f;
	iBulletAmount = 20;
	fBulletSpread = 4.0f;
	fShotrate = 1.0f;
	fShotPause = 1.0f;
	MaxAmmo = 6;
	WeaponType = EWeaponType::WT_SINGLE;
	TargetAnimation = ETargetAnimation::TA_RIFLE;
	fAI_AttackRange = 1000.0f;
}

