#include "Aimbot.h"

#include "../Vars.h"

void CFeatures_Aimbot::Run(C_TerrorPlayer* pLocal, CUserCmd* cmd)
{
	if (!Vars::Aimbot::bEnabled)
		return;

	if (pLocal->m_isIncapacitated() || pLocal->m_isHangingFromLedge())
		return;

	Target_t target = {};

	if (!GetTarget(pLocal, cmd, target))
		return;

	Vector vAngle = U::Math.GetAngleToPosition(pLocal->EyePosition(), target.m_vPos);
	U::Math.ClampAngles(vAngle);

	if (Vars::Aimbot::bSilent)
	{
		G::Util.FixMovement(vAngle, cmd);
		cmd->viewangles = vAngle;
	}
	else
	{
		cmd->viewangles = vAngle;
		I::EngineClient->SetViewAngles(vAngle);
	}

	if (Vars::Aimbot::bAutoFire)
		cmd->buttons |= IN_ATTACK;
}

bool CFeatures_Aimbot::GetTarget(C_TerrorPlayer* pLocal, CUserCmd* cmd, Target_t& out)
{
	const int nLocalIndex = I::EngineClient->GetLocalPlayer();
	const Vector vEyePos = pLocal->EyePosition();
	const bool bLocalSurvivor = (pLocal->GetTeamNumber() == TEAM_SURVIVOR);

	for (int n = 1; n < (I::ClientEntityList->GetMaxEntities() + 1); n++)
	{
		if (n == nLocalIndex)
			continue;

		IClientEntity* pEntity = I::ClientEntityList->GetClientEntity(n);

		if (!pEntity || pEntity->IsDormant())
			continue;

		ClientClass* pCC = pEntity->GetClientClass();

		if (!pCC)
			continue;

		C_BaseEntity* pBase = pEntity->As<C_BaseEntity*>();
		Vector vTargetPos = {};
		bool bValid = false;

		switch (pCC->m_ClassID)
		{
			case CTerrorPlayer:
			case SurvivorBot:
			{
				C_TerrorPlayer* pPlayer = pEntity->As<C_TerrorPlayer*>();

				if (pPlayer->deadflag() || pPlayer->m_lifeState() != 0)
					break;

				const bool bIsSurvivor = (pPlayer->GetTeamNumber() == TEAM_SURVIVOR);

				if (bLocalSurvivor && bIsSurvivor && !Vars::Aimbot::bTargetSurvivors)
					break;

				if (bLocalSurvivor && !bIsSurvivor)
				{
					const int nZombieClass = pPlayer->m_zombieClass();

					if (nZombieClass == CLASS_TANK)
					{
						if (!Vars::Aimbot::bTargetSpecial)
							break;
					}
					else if (nZombieClass >= CLASS_SMOKER && nZombieClass <= CLASS_CHARGER)
					{
						if (!Vars::Aimbot::bTargetSpecial)
							break;
					}

					if (pPlayer->m_isGhost())
						break;
				}

				vTargetPos = GetTargetPos(pPlayer, Vars::Aimbot::nHitbox);
				bValid = true;
				break;
			}
			case Infected:
			{
				if (!Vars::Aimbot::bTargetInfected)
					break;

				C_Infected* pInfected = pEntity->As<C_Infected*>();

				if (!G::Util.IsInfectedAlive(pInfected->GetSolidFlags(), pInfected->m_nSequence()))
					break;

				vTargetPos = GetTargetPos(pInfected, 0);
				bValid = true;
				break;
			}
			case Witch:
			{
				if (!Vars::Aimbot::bTargetWitch)
					break;

				C_Witch* pWitch = pEntity->As<C_Witch*>();
				vTargetPos = GetTargetPos(pWitch, 0);
				bValid = true;
				break;
			}
			default:
				break;
		}

		if (!bValid || vTargetPos.IsZero())
			continue;

		const float flFOV = U::Math.GetFovBetween(cmd->viewangles, U::Math.GetAngleToPosition(vEyePos, vTargetPos));

		if (flFOV > Vars::Aimbot::flFOV)
			continue;

		if (Vars::Aimbot::bVisCheck && !IsVisible(pLocal, pBase, vTargetPos))
			continue;

		const float flDist = vEyePos.DistTo(vTargetPos);

		bool bBetter = false;

		if (Vars::Aimbot::nPriority == 0)
			bBetter = (flFOV < out.m_flFOV);
		else
			bBetter = (flDist < out.m_flDist);

		if (bBetter)
		{
			out.m_pEntity = pBase;
			out.m_vPos = vTargetPos;
			out.m_flFOV = flFOV;
			out.m_flDist = flDist;
		}
	}

	return (out.m_pEntity != nullptr);
}

bool CFeatures_Aimbot::IsVisible(C_TerrorPlayer* pLocal, C_BaseEntity* pEntity, const Vector& vPos)
{
	trace_t tr = {};
	CTraceFilterHitAll filter(pLocal);
	G::Util.Trace(pLocal->EyePosition(), vPos, MASK_SHOT, &filter, &tr);

	return (tr.m_pEnt == pEntity || tr.fraction > 0.97f);
}

Vector CFeatures_Aimbot::GetTargetPos(C_BaseEntity* pEntity, int nHitbox)
{
	C_BaseAnimating* pAnim = pEntity->GetBaseAnimating();

	if (!pAnim)
		return pEntity->WorldSpaceCenter();

	Vector vPos = {};

	if (nHitbox == 0)
	{
		if (pAnim->GetHitboxPositionByGroup(HITGROUP_HEAD, vPos))
			return vPos;
	}
	else if (nHitbox == 1)
	{
		if (pAnim->GetHitboxPositionByGroup(HITGROUP_CHEST, vPos))
			return vPos;
		if (pAnim->GetHitboxPositionByGroup(HITGROUP_STOMACH, vPos))
			return vPos;
	}

if (pAnim->GetHitboxPositionByGroup(HITGROUP_HEAD, vPos))
		return vPos;
	if (pAnim->GetHitboxPositionByGroup(HITGROUP_CHEST, vPos))
		return vPos;

	return pEntity->WorldSpaceCenter();
}
