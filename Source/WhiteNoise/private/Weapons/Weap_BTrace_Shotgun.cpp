// Copyright 2016 Carsten Zarbock / Rebound-Software
#include "WhiteNoise.h"
#include "Weap_BTrace_Shotgun.h"

AShotgun::AShotgun()
{
	/* Default values, edit in BP class */
	fSplatDamageRange = 500.00f;
	fAnimDamageRange = 700.0f;
	fCrawlDamageRange = 1000.0f;
	fMaxRange = 10000.0f;
	iBulletAmount = 20;
	fBulletSpread = 4.0f;
	Shotrate = 1.0f;
	ShotPause = 1.0f;
	MaxAmmo = 6;
	WeaponType = EWeaponType::WT_SINGLE;
	TargetAnimation = ETargetAnimation::TA_RIFLE;
	AIAttackRange = 1000.0f;
}

