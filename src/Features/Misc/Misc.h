#pragma once

#include "../../SDK/SDK.h"

class CFeatures_Triggerbot
{
public:
	void Run(C_TerrorPlayer* pLocal, CUserCmd* cmd);
};

class CFeatures_Misc
{
public:
	void Bhop(C_TerrorPlayer* pLocal, CUserCmd* cmd);
	void AutoStrafe(C_TerrorPlayer* pLocal, CUserCmd* cmd);
	void AutoDeadstop(C_TerrorPlayer* pLocal, CUserCmd* cmd);
};

namespace F { inline CFeatures_Triggerbot Triggerbot; inline CFeatures_Misc Misc; }
