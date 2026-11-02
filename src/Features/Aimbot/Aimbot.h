#pragma once

#include "../../SDK/SDK.h"

class CFeatures_Aimbot
{
public:
	void Run(C_TerrorPlayer* pLocal, CUserCmd* cmd);

private:
	struct Target_t
	{
		C_BaseEntity* m_pEntity = nullptr;
		Vector m_vPos = {};
		float m_flFOV = 999.0f;
		float m_flDist = 999999.0f;
	};

	bool GetTarget(C_TerrorPlayer* pLocal, CUserCmd* cmd, Target_t& out);
	bool IsVisible(C_TerrorPlayer* pLocal, C_BaseEntity* pEntity, const Vector& vPos);
	Vector GetTargetPos(C_BaseEntity* pEntity, int nHitbox);
};

namespace F { inline CFeatures_Aimbot Aimbot; }
