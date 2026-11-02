#include "Misc.h"

#include "../Vars.h"

void CFeatures_Triggerbot::Run(C_TerrorPlayer* pLocal, CUserCmd* cmd)
{
	if (!Vars::Triggerbot::bEnabled)
		return;

	if (pLocal->m_isIncapacitated() || pLocal->m_isHangingFromLedge())
		return;

	C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();

	if (!pWeapon)
		return;

	Vector vForward = {};
	U::Math.AngleVectors(cmd->viewangles, &vForward);

	const Vector vStart = pLocal->EyePosition();
	const Vector vEnd = vStart + (vForward * 8192.0f);

	trace_t tr = {};
	CTraceFilterHitAll filter(pLocal);
	G::Util.Trace(vStart, vEnd, MASK_SHOT, &filter, &tr);

	if (tr.fraction == 1.0f || !tr.m_pEnt)
		return;

	IClientEntity* pEntity = reinterpret_cast<IClientEntity*>(tr.m_pEnt);

	if (pEntity->IsDormant())
		return;

	ClientClass* pCC = pEntity->GetClientClass();

	if (!pCC)
		return;

	bool bShouldFire = false;

	switch (pCC->m_ClassID)
	{
		case Infected:
		case Witch:
			bShouldFire = true;
			break;
		case CTerrorPlayer:
		case SurvivorBot:
		{
			if (Vars::Triggerbot::bInfectedOnly)
			{
				C_TerrorPlayer* pPlayer = pEntity->As<C_TerrorPlayer*>();
				bShouldFire = (pPlayer->GetTeamNumber() != pLocal->GetTeamNumber());
			}
			else
			{
				bShouldFire = true;
			}
			break;
		}
		default:
			break;
	}

	if (bShouldFire)
		cmd->buttons |= IN_ATTACK;
}

void CFeatures_Misc::Bhop(C_TerrorPlayer* pLocal, CUserCmd* cmd)
{
	if (!Vars::Misc::bBhop)
		return;

	if (pLocal->m_isIncapacitated() || pLocal->m_isHangingFromLedge())
		return;

	if (pLocal->m_MoveType() == MOVETYPE_LADDER || pLocal->m_MoveType() == MOVETYPE_NOCLIP)
		return;

	if (cmd->buttons & IN_JUMP)
	{
		if (!(pLocal->m_fFlags() & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;
	}
}

void CFeatures_Misc::AutoStrafe(C_TerrorPlayer* pLocal, CUserCmd* cmd)
{
	if (!Vars::Misc::bAutoStrafe)
		return;

	if (pLocal->m_fFlags() & FL_ONGROUND)
		return;

	if (pLocal->m_MoveType() == MOVETYPE_LADDER || pLocal->m_MoveType() == MOVETYPE_NOCLIP)
		return;

	if (cmd->mousedx > 1)
	{
		cmd->sidemove = 450.0f;
	}
	else if (cmd->mousedx < -1)
	{
		cmd->sidemove = -450.0f;
	}
}

void CFeatures_Misc::AutoDeadstop(C_TerrorPlayer* pLocal, CUserCmd* cmd)
{
	if (!Vars::Misc::bAutoDeadstop)
		return;

	if (pLocal->GetTeamNumber() != 2)
		return;

	const Vector vLocalPos = pLocal->GetAbsOrigin();

	for (int i = 1; i <= I::EngineClient->GetMaxClients(); i++)
	{
		IClientEntity* pEntity = I::ClientEntityList->GetClientEntity(i);

		if (!pEntity || pEntity->IsDormant())
			continue;

		C_TerrorPlayer* pPlayer = pEntity->As<C_TerrorPlayer*>();

		if (pPlayer->GetTeamNumber() != 3 || pPlayer->deadflag() || pPlayer->m_isGhost())
			continue;

		int nClass = pPlayer->m_zombieClass();

		if (nClass != 3 && nClass != 5)
			continue;

		if (pPlayer->m_fFlags() & FL_ONGROUND)
			continue;

		float flDist = vLocalPos.DistTo(pPlayer->GetAbsOrigin());

		if (flDist < 110.0f)
		{
			cmd->buttons |= IN_ATTACK2;
			return;
		}
	}
}
