#include "ESP.h"

#include "../Vars.h"
#include <cmath>

static const wchar_t* GetMeleeWeaponName(C_BaseEntity* pEntity)
{
	const model_t* pModel = pEntity->GetModel();

	if (!pModel)
		return L"melee";

	const char* szModelName = I::ModelInfo->GetModelName(pModel);

	if (!szModelName)
		return L"melee";

const char* pSlash = strrchr(szModelName, '/');
	if (!pSlash) pSlash = strrchr(szModelName, '\\');
	if (!pSlash) return L"melee";

	const char* szName = pSlash + 1;

if ((szName[0] == 'v' || szName[0] == 'w') && szName[1] == '_')
		szName += 2;

	static const struct { const char* model; const wchar_t* display; } s_MeleeNames[] = {
		{ "fireaxe",         L"Fire Axe" },
		{ "katana",          L"Katana" },
		{ "machete",         L"Machete" },
		{ "cricket_bat",     L"Cricket Bat" },
		{ "crowbar",         L"Crowbar" },
		{ "electric_guitar", L"Guitar" },
		{ "frying_pan",      L"Frying Pan" },
		{ "golfclub",        L"Golf Club" },
		{ "baseball_bat",    L"Baseball Bat" },
		{ "bat",             L"Baseball Bat" },
		{ "tonfa",           L"Nightstick" },
		{ "pitchfork",       L"Pitchfork" },
		{ "shovel",          L"Shovel" },
		{ "knife",           L"Knife" },
		{ "riotshield",      L"Riot Shield" },
		{ "chainsaw",        L"Chainsaw" },
	};

	for (const auto& entry : s_MeleeNames)
	{
		if (strstr(szName, entry.model))
			return entry.display;
	}

	return L"Melee";
}

void CFeatures_ESP::Render()
{
	if (!Vars::ESP::bEnabled || !I::EngineClient->IsInGame() || I::EngineVGui->IsGameUIVisible())
		return;

	const int nLocalIndex = I::EngineClient->GetLocalPlayer();

	C_TerrorPlayer* pLocal = I::ClientEntityList->GetClientEntity(nLocalIndex)->As<C_TerrorPlayer*>();

	if (!pLocal)
		return;

	player_info_t pi;

	int x, y, w, h;
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
		
		switch (pCC->m_ClassID)
		{
			case CTerrorPlayer:
			case SurvivorBot:
			{
				if (!Vars::ESP::bPlayers)
					break;

				C_TerrorPlayer* pPlayer = pEntity->As<C_TerrorPlayer*>();

				if (pPlayer->deadflag() || pPlayer->m_isGhost() || !GetBounds(pPlayer, x, y, w, h))
					break;

				const int nHealth = pPlayer->GetActualHealth();
				const int nMaxHealth = pPlayer->GetMaxHealth();
				const bool bIsSurvivor = (pPlayer->GetTeamNumber() == TEAM_SURVIVOR);

				if (!bIsSurvivor && !Vars::ESP::bSpecial)
					break;

				const Color clrHealth = G::Util.GetHealthColor(nHealth, nMaxHealth);
				const Color clrTeam = bIsSurvivor ? Color(15, 150, 150, 255) : Color(150, 15, 15, 255);

				if (Vars::ESP::bBox)
				{
					G::Draw.OutlinedRect(x, y, w, h, clrTeam);
					G::Draw.OutlinedRect(x - 1, y - 1, w + 2, h + 2, { 0, 0, 0, 80 });
				}

				if (Vars::ESP::bHealthBar)
				{
					const int nBarH = h;
					const int nBarX = x - 6;
					const float flRatio = U::Math.Clamp(static_cast<float>(nHealth) / static_cast<float>(nMaxHealth), 0.0f, 1.0f);
					const int nFill = static_cast<int>(nBarH * flRatio);

					G::Draw.Rect(nBarX, y, 4, nBarH, { 0, 0, 0, 120 });
					G::Draw.Rect(nBarX, y + (nBarH - nFill), 4, nFill, clrHealth);
					G::Draw.OutlinedRect(nBarX, y, 4, nBarH, { 0, 0, 0, 180 });
				}

				int nDrawX = x + (w / 2);
				int nDrawY = y - 2;

				if (Vars::ESP::bName && I::EngineClient->GetPlayerInfo(n, &pi))
				{
					nDrawY -= G::Draw.GetFontHeight(EFonts::ESP_NAME);
					G::Draw.String(EFonts::ESP_NAME, nDrawX, nDrawY, clrTeam, TXT_CENTERX, pi.name);
				}

				nDrawY = y + h + 2;

				G::Draw.String(EFonts::ESP, nDrawX, nDrawY, clrHealth, TXT_CENTERX, L"%i / %ihp", nHealth, nMaxHealth);
				nDrawY += G::Draw.GetFontHeight(EFonts::ESP);

				if (!bIsSurvivor)
				{
					const int nZombieClass = pPlayer->m_zombieClass();
					const wchar_t* szClass = L"";

					switch (nZombieClass)
					{
						case CLASS_SMOKER:  szClass = L"Smoker"; break;
						case CLASS_BOOMER:  szClass = L"Boomer"; break;
						case CLASS_HUNTER:  szClass = L"Hunter"; break;
						case CLASS_SPITTER: szClass = L"Spitter"; break;
						case CLASS_JOCKEY:  szClass = L"Jockey"; break;
						case CLASS_CHARGER: szClass = L"Charger"; break;
						case CLASS_TANK:    szClass = L"Tank"; break;
					}

					if (szClass[0])
					{
						G::Draw.String(EFonts::ESP, nDrawX, nDrawY, { 230, 130, 130, 255 }, TXT_CENTERX, szClass);
						nDrawY += G::Draw.GetFontHeight(EFonts::ESP);
					}
				}

				if (bIsSurvivor && Vars::ESP::bWeapon)
				{
					C_BaseCombatWeapon* pWeapon = pPlayer->GetActiveWeapon();

					if (pWeapon)
					{
						C_TerrorWeapon* pTWeapon = reinterpret_cast<C_TerrorWeapon*>(pWeapon);
						ClientClass* pWeaponCC = reinterpret_cast<IClientEntity*>(pWeapon)->GetClientClass();
						bool bIsMelee = false;

						if (pWeaponCC)
						{
							const char* szClassName = pWeaponCC->m_pNetworkName;
							if (szClassName && (strstr(szClassName, "Melee") || strstr(szClassName, "melee")))
								bIsMelee = true;
						}

						if (bIsMelee)
						{
							G::Draw.String(EFonts::ESP_WEAPON, nDrawX, nDrawY, { 204, 204, 204, 255 }, TXT_CENTERX, GetMeleeWeaponName(pWeapon));
						}
						else
						{
							G::Draw.String(EFonts::ESP_WEAPON, nDrawX, nDrawY, { 204, 204, 204, 255 }, TXT_CENTERX, pWeapon->GetPrintName());
						}
						nDrawY += G::Draw.GetFontHeight(EFonts::ESP_WEAPON);
					}
				}

				if (Vars::ESP::bDistance)
				{
					const float flDist = pLocal->GetAbsOrigin().DistTo(pPlayer->GetAbsOrigin()) * 0.01905f;
					G::Draw.String(EFonts::ESP, nDrawX, nDrawY, { 180, 180, 180, 255 }, TXT_CENTERX, L"%.0fm", flDist);
					nDrawY += G::Draw.GetFontHeight(EFonts::ESP);
				}

				if (Vars::ESP::bSnaplines)
				{
					G::Draw.Line(G::Draw.m_nScreenW / 2, G::Draw.m_nScreenH, x + w / 2, y + h, clrTeam);
				}

				break;
			}
			case Infected:
			{
				if (!Vars::ESP::bInfected)
					break;

				C_Infected* pInfected = pEntity->As<C_Infected*>();

				if (!G::Util.IsInfectedAlive(pInfected->GetSolidFlags(), pInfected->m_nSequence()))
					break;

				Vector vScreen = {};

				if (!G::Util.W2S(pInfected->GetAbsOrigin(), vScreen))
					break;

				G::Draw.String(EFonts::ESP, static_cast<int>(vScreen.x), static_cast<int>(vScreen.y), { 200, 180, 80, 255 }, TXT_CENTERX, L"infected");

				break;
			}
			case Witch:
			{
				if (!Vars::ESP::bWitch)
					break;

				C_Witch* pWitch = pEntity->As<C_Witch*>();

				if (!GetBounds(pWitch, x, y, w, h))
					break;

				const int nDrawX = x + (w / 2);
				int nDrawY = y - G::Draw.GetFontHeight(EFonts::ESP_NAME) - 2;

				G::Draw.String(EFonts::ESP_NAME, nDrawX, nDrawY, { 255, 50, 200, 255 }, TXT_CENTERX, L"WITCH");

				if (Vars::ESP::bBox)
				{
					G::Draw.OutlinedRect(x, y, w, h, { 255, 50, 200, 255 });
				}

				if (Vars::ESP::bDistance)
				{
					const float flDist = pLocal->GetAbsOrigin().DistTo(pWitch->GetAbsOrigin()) * 0.01905f;
					G::Draw.String(EFonts::ESP, nDrawX, y + h + 2, { 180, 180, 180, 255 }, TXT_CENTERX, L"%.0fm", flDist);
				}

				break;
			}
			case CWeaponSpawn:
			{
				if (!Vars::ESP::bItems)
					break;

				C_WeaponSpawn* pSpawn = pEntity->As<C_WeaponSpawn*>();

				if (!GetBounds(pSpawn, x, y, w, h))
					break;

				const int nID = U::Math.Clamp(pSpawn->GetWeaponID(), 0, 38);
				G::Draw.String(EFonts::ESP, x + (w / 2), y + (h / 2), g_aSpawnInfo[nID].m_Color, TXT_CENTERX, g_aSpawnInfo[nID].m_szName);

				break;
			}
			case CPropMountedGun:
			case CPropMinigun:
			{
				if (!Vars::ESP::bItems)
					break;

				C_BaseMountedWeapon* pMounted = pEntity->As<C_BaseMountedWeapon*>();

				if (!GetBounds(pMounted, x, y, w, h))
					break;

				const int nDrawX = x + (w / 2);
				int nDrawY = y + (h / 2);

				G::Draw.String(EFonts::ESP_NAME, nDrawX, nDrawY, { 204, 204, 204, 255 }, TXT_CENTERX, L"mounted weapon");
				nDrawY += G::Draw.GetFontHeight(EFonts::ESP_NAME);

				G::Draw.String(EFonts::ESP, nDrawX, nDrawY, { 204, 204, 204, 255 }, TXT_CENTERX, L"heat: %.1f", U::Math.Clamp(pMounted->m_heat() * 100.0f, 0.0f, 100.0f));
				nDrawY += G::Draw.GetFontHeight(EFonts::ESP);

				if (pMounted->m_overheated())
				{
					G::Draw.String(EFonts::ESP, nDrawX, nDrawY, { 240, 230, 140, 255 }, TXT_CENTERX, L"OVERHEATED");
					nDrawY += G::Draw.GetFontHeight(EFonts::ESP);
				}

				break;
			}
			default:
				break;
		}
	}

	if (Vars::Aimbot::bEnabled && Vars::Aimbot::bFOVCircle)
	{
		const int nCenterX = G::Draw.m_nScreenW / 2;
		const int nCenterY = G::Draw.m_nScreenH / 2;

		float flFOVGame = 90.0f;
		if (Vars::Misc::flFOVOverride > 0.0f)
			flFOVGame = Vars::Misc::flFOVOverride;

		const float flRadius = tanf(Vars::Aimbot::flFOV * 3.14159f / 180.0f) /
			tanf(flFOVGame * 3.14159f / 360.0f) * (G::Draw.m_nScreenW / 2.0f);

		G::Draw.OutlinedCircle(nCenterX, nCenterY, static_cast<int>(flRadius), 64, { 100, 80, 200, 120 });
	}
}

bool CFeatures_ESP::GetBounds(C_BaseEntity* pBaseEntity, int& x, int& y, int& w, int& h)
{
	Vector vPoints[8];
	U::Math.BuildTransformedBox(vPoints, pBaseEntity->m_vecMins(), pBaseEntity->m_vecMaxs(), pBaseEntity->RenderableToWorldTransform());

	Vector flb, brt, blb, frt, frb, brb, blt, flt;
	if (G::Util.W2S(vPoints[3], flb) && G::Util.W2S(vPoints[5], brt)
		&& G::Util.W2S(vPoints[0], blb) && G::Util.W2S(vPoints[4], frt)
		&& G::Util.W2S(vPoints[2], frb) && G::Util.W2S(vPoints[1], brb)
		&& G::Util.W2S(vPoints[6], blt) && G::Util.W2S(vPoints[7], flt)
		&& G::Util.W2S(vPoints[6], blt) && G::Util.W2S(vPoints[7], flt))
	{
		const Vector vTransformed[8] = { flb, brt, blb, frt, frb, brb, blt, flt };

		float left = flb.x;
		float top = flb.y;
		float righ = flb.x;
		float bottom = flb.y;

		for (int n = 1; n < 8; n++)
		{
			if (left > vTransformed[n].x)
				left = vTransformed[n].x;

			if (top < vTransformed[n].y)
				top = vTransformed[n].y;

			if (righ < vTransformed[n].x)
				righ = vTransformed[n].x;

			if (bottom > vTransformed[n].y)
				bottom = vTransformed[n].y;
		}

		x = static_cast<int>(left);
		y = static_cast<int>(bottom);
		w = static_cast<int>(righ - left);
		h = static_cast<int>(top - bottom);

		return !(x > G::Draw.m_nScreenW || (x + w) < 0 || y > G::Draw.m_nScreenH || (y + h) < 0);
	}

	return false;
}