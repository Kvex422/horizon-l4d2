#include "ModelRender.h"

#include "../../Features/Vars.h"

using namespace Hooks;

static IMaterial* s_pMatFlat = nullptr;

static IMaterial* GetFlatMaterial()
{
	if (!s_pMatFlat)
	{
		s_pMatFlat = G::Util.CreateMaterial(
			"\"UnlitGeneric\"\n"
			"{\n"
			"	\"$basetexture\" \"vgui/white_additive\"\n"
			"	\"$model\" \"1\"\n"
			"	\"$flat\" \"1\"\n"
			"	\"$nocull\" \"1\"\n"
			"	\"$selfillum\" \"1\"\n"
			"	\"$halflambert\" \"1\"\n"
			"	\"$nofog\" \"1\"\n"
			"	\"$znearer\" \"0\"\n"
			"	\"$wireframe\" \"0\"\n"
			"}\n"
		);
	}

	return s_pMatFlat;
}

void __fastcall ModelRender::ForcedMaterialOverride::Detour(void* ecx, void* edx, IMaterial* newMaterial, OverrideType_t nOverrideType)
{
	Table.Original<FN>(Index)(ecx, edx, newMaterial, nOverrideType);
}

void __fastcall ModelRender::DrawModelExecute::Detour(void* ecx, void* edx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (Vars::Chams::bEnabled && pInfo.entity_index > 0)
	{
		IClientEntity* pEntity = I::ClientEntityList->GetClientEntity(pInfo.entity_index);

		if (pEntity && !pEntity->IsDormant())
		{
			ClientClass* pCC = pEntity->GetClientClass();

			if (pCC)
			{
				bool bDraw = false;
				float r = 1.0f, g = 1.0f, b = 1.0f;

				switch (pCC->m_ClassID)
				{
					case CTerrorPlayer:
					case SurvivorBot:
					{
						if (!Vars::Chams::bPlayers)
							break;

						C_TerrorPlayer* pPlayer = pEntity->As<C_TerrorPlayer*>();

						if (pPlayer->deadflag())
							break;

						if (pPlayer->GetTeamNumber() == TEAM_SURVIVOR)
						{
							r = 0.1f; g = 0.6f; b = 0.9f;
						}
						else
						{
							r = 0.9f; g = 0.2f; b = 0.1f;
						}

						bDraw = true;
						break;
					}
					case Infected:
					{
						if (!Vars::Chams::bInfected)
							break;

						r = 0.8f; g = 0.6f; b = 0.1f;
						bDraw = true;
						break;
					}
					case Witch:
					{
						r = 1.0f; g = 0.0f; b = 1.0f;
						bDraw = true;
						break;
					}
					default:
						break;
				}

				if (bDraw)
				{
					IMaterial* pMat = GetFlatMaterial();

					if (pMat && !IsErrorMaterial(pMat))
					{
						if (Vars::Chams::bIgnoreZ)
						{
							pMat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							pMat->ColorModulate(r * 0.4f, g * 0.4f, b * 0.4f);
							I::ModelRender->ForcedMaterialOverride(pMat);
							Table.Original<FN>(Index)(ecx, edx, state, pInfo, pCustomBoneToWorld);
						}

						pMat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
						pMat->ColorModulate(r, g, b);
						I::ModelRender->ForcedMaterialOverride(pMat);
						Table.Original<FN>(Index)(ecx, edx, state, pInfo, pCustomBoneToWorld);

						I::ModelRender->ForcedMaterialOverride(nullptr);
						return;
					}
				}
			}
		}
	}

	Table.Original<FN>(Index)(ecx, edx, state, pInfo, pCustomBoneToWorld);
}

void ModelRender::Init()
{
	XASSERT(Table.Init(I::ModelRender) == false);
	XASSERT(Table.Hook(&ForcedMaterialOverride::Detour, ForcedMaterialOverride::Index) == false);
	XASSERT(Table.Hook(&DrawModelExecute::Detour, DrawModelExecute::Index) == false);
}