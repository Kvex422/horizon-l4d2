#pragma once

namespace Vars
{
	namespace Aimbot
	{
		inline bool bEnabled = false;
		inline bool bFOVCircle = true;
		inline bool bSilent = true;
		inline float flFOV = 15.0f;
		inline bool bVisCheck = true;
		inline bool bAutoFire = false;
		inline int nPriority = 0;
		inline int nHitbox = 0;
		inline bool bTargetInfected = true;
		inline bool bTargetSpecial = true;
		inline bool bTargetSurvivors = false;
		inline bool bTargetWitch = true;
	}

	namespace Triggerbot
	{
		inline bool bEnabled = false;
		inline bool bInfectedOnly = true;
		inline int nDelay = 0;
	}

	namespace ESP
	{
		inline bool bEnabled = true;
		inline bool bPlayers = true;
		inline bool bInfected = false;
		inline bool bSpecial = true;
		inline bool bWitch = true;
		inline bool bItems = true;
		inline bool bBox = false;
		inline bool bHealthBar = true;
		inline bool bDistance = true;
		inline bool bName = true;
		inline bool bWeapon = true;
		inline bool bSnaplines = false;
		inline bool bGlow = false;
	}

	namespace Chams
	{
		inline bool bEnabled = false;
		inline bool bPlayers = true;
		inline bool bInfected = false;
		inline bool bIgnoreZ = true;
	}

	namespace Misc
	{
		inline bool bBhop = false;
		inline bool bAutoStrafe = false;
		inline bool bAutoDeadstop = false;
		inline bool bNoSpread = true;
		inline bool bNoRecoil = true;
		inline bool bSpeedhack = false;
		inline int nSpeedhackFactor = 5;
		inline bool bNoFog = false;
		inline float flFOVOverride = 0.0f;
		inline float flViewmodelFOV = 0.0f;
		inline bool bFakelag = false;
		inline int nFakelagTicks = 7;
		inline bool bHitmarker = true;
		inline bool bHitSound = true;
	}
}
