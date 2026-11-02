#include "ClientMode.h"

#include "../../Features/Vars.h"
#include "../../Features/EnginePrediction/EnginePrediction.h"
#include "../../Features/NoSpread/NoSpread.h"
#include "../../Features/Aimbot/Aimbot.h"
#include "../../Features/Misc/Misc.h"

using namespace Hooks;

bool __fastcall ClientMode::ShouldDrawFog::Detour(void* ecx, void* edx)
{
	if (Vars::Misc::bNoFog)
		return false;

	return Table.Original<FN>(Index)(ecx, edx);
}

bool __fastcall ClientMode::CreateMove::Detour(void* ecx, void* edx, float input_sample_frametime, CUserCmd* cmd)
{
	if (!cmd || !cmd->command_number)
		return Table.Original<FN>(Index)(ecx, edx, input_sample_frametime, cmd);

	if (Table.Original<FN>(Index)(ecx, edx, input_sample_frametime, cmd))
		I::Prediction->SetLocalViewAngles(cmd->viewangles);

	uintptr_t _ebp; __asm mov _ebp, ebp;
	bool* pSendPacket = (bool*)(***(uintptr_t***)_ebp - 0x1D);

	C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();

	if (pLocal && !pLocal->deadflag())
	{
		C_TerrorWeapon* pWeapon = pLocal->GetActiveWeapon()->As<C_TerrorWeapon*>();

		F::Misc.Bhop(pLocal, cmd);
		F::Misc.AutoStrafe(pLocal, cmd);
		F::Misc.AutoDeadstop(pLocal, cmd);

		if (Vars::Misc::bFakelag && pSendPacket)
		{
			static int nTicksChoked = 0;

			bool bForceSync = pLocal->m_MoveType() == MOVETYPE_LADDER ||
				pLocal->m_isIncapacitated() ||
				pLocal->m_isHangingFromLedge();

			if (bForceSync)
			{
				*pSendPacket = true;
				nTicksChoked = 0;
			}
			else if (nTicksChoked < Vars::Misc::nFakelagTicks)
			{
				*pSendPacket = false;
				nTicksChoked++;
			}
			else
			{
				*pSendPacket = true;
				nTicksChoked = 0;
			}
		}

		if (pWeapon)
		{
			F::EnginePrediction.Start(pLocal, cmd);
			{
				F::NoSpread.Run(pLocal, pWeapon, cmd);
				F::Aimbot.Run(pLocal, cmd);
				F::Triggerbot.Run(pLocal, cmd);
			}
			F::EnginePrediction.Finish(pLocal, cmd);
		}
	}

	return false;
}

void __fastcall ClientMode::DoPostScreenSpaceEffects::Detour(void* ecx, void* edx, const void* pSetup)
{
	Table.Original<FN>(Index)(ecx, edx, pSetup);
}

float __fastcall ClientMode::GetViewModelFOV::Detour(void* ecx, void* edx)
{
	float flFOV = Table.Original<FN>(Index)(ecx, edx);

	if (Vars::Misc::flViewmodelFOV > 0.0f)
		flFOV = Vars::Misc::flViewmodelFOV;

	return flFOV;
}

void ClientMode::Init()
{
	XASSERT(Table.Init(I::ClientMode) == false);
	XASSERT(Table.Hook(&ShouldDrawFog::Detour, ShouldDrawFog::Index) == false);
	XASSERT(Table.Hook(&CreateMove::Detour, CreateMove::Index) == false);
	XASSERT(Table.Hook(&DoPostScreenSpaceEffects::Detour, DoPostScreenSpaceEffects::Index) == false);
	XASSERT(Table.Hook(&GetViewModelFOV::Detour, GetViewModelFOV::Index) == false);
}