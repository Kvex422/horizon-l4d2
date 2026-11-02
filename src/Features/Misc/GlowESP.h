#pragma once

#include "../../SDK/SDK.h"
#include "../../Features/Vars.h"

struct GlowObjectDefinition_t
{
	C_BaseEntity* m_pEntity;
	Vector m_vGlowColor;
	float m_flGlowAlpha;
	char pad0[4];
	float m_flSomething;
	float m_flBloomAmount;
	float m_flSomethingElse;
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloomRender;
	char pad1[1];
	int m_nFullBloomStencilTestValue;
	int m_nGlowStyle;
	int m_nSplitScreenSlot;
	int m_nNextFreeSlot;

	static constexpr int END_OF_FREE_LIST = -1;
	static constexpr int ENTRY_IN_USE = -2;

	bool IsUnused() const { return m_nNextFreeSlot != ENTRY_IN_USE; }
};

class CGlowObjectManager
{
public:
	GlowObjectDefinition_t* m_GlowObjectDefinitions;
	int max_size;
	int pad;
	int size;
};

namespace GlowESP
{
	inline CGlowObjectManager** g_ppGlowManager = nullptr;

	inline void Run()
	{
		if (!Vars::ESP::bGlow)
			return;

		if (!g_ppGlowManager)
		{
			static auto dwPattern = U::Pattern.Find("client.dll", "0F 11 05 ? ? ? ? 83 C8 01");

			if (!dwPattern)
				return;

			g_ppGlowManager = reinterpret_cast<CGlowObjectManager**>(dwPattern + 3);
		}

		if (!g_ppGlowManager || !*g_ppGlowManager)
			return;

		C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(I::EngineClient->GetLocalPlayer())->As<C_TerrorPlayer*>();

		if (!pLocal)
			return;

		CGlowObjectManager* pGlowManager = *g_ppGlowManager;

		for (int i = 0; i < pGlowManager->size; i++)
		{
			GlowObjectDefinition_t& glowObj = pGlowManager->m_GlowObjectDefinitions[i];

			if (glowObj.IsUnused())
				continue;

			C_BaseEntity* pEntity = glowObj.m_pEntity;

			if (!pEntity || pEntity == pLocal)
				continue;

			if (pEntity->IsDormant())
				continue;

			int nClassID = pEntity->GetClientClass()->m_ClassID;
			bool bApply = false;
			Vector vColor = { 1.0f, 1.0f, 1.0f };

			if (nClassID == 64 || nClassID == 92)
			{
				auto* pPlayer = pEntity->As<C_TerrorPlayer*>();

				if (pPlayer->GetTeamNumber() == 2)
				{
					vColor = { 0.2f, 0.5f, 1.0f };
					bApply = true;
				}
				else if (pPlayer->GetTeamNumber() == 3)
				{
					vColor = { 1.0f, 0.2f, 0.2f };
					bApply = true;
				}
			}
			else if (nClassID == 29)
			{
				vColor = { 1.0f, 0.8f, 0.0f };
				bApply = Vars::ESP::bInfected;
			}
			else if (nClassID == 99)
			{
				vColor = { 1.0f, 0.0f, 1.0f };
				bApply = true;
			}

			if (bApply)
			{
				glowObj.m_vGlowColor = vColor;
				glowObj.m_flGlowAlpha = 0.85f;
				glowObj.m_bRenderWhenOccluded = true;
				glowObj.m_bRenderWhenUnoccluded = false;
				glowObj.m_flBloomAmount = 1.0f;
				glowObj.m_nGlowStyle = 0;
			}
		}
	}
}
