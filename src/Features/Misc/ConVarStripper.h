#pragma once

#include "../../SDK/SDK.h"

#define FCVAR_CHEAT              (1 << 1)
#define FCVAR_DEVELOPMENTONLY    (1 << 1)
#define FCVAR_HIDDEN             (1 << 4)
#define FCVAR_NOT_CONNECTED      (1 << 22)
#define FCVAR_REPLICATED         (1 << 13)

class ConCommandBase
{
public:
	void*           m_pVTable;
	ConCommandBase* m_pNext;
	bool            m_bRegistered;
	const char*     m_pszName;
	const char*     m_pszHelpString;
	int             m_nFlags;
};

class ConVar : public ConCommandBase
{
public:
	void*       m_pParent;
	const char* m_pszDefaultValue;
	char*       m_pszString;
	int         m_nStringLength;
	float       m_fValue;
	int         m_nValue;
	bool        m_bHasMin;
	float       m_fMinVal;
	bool        m_bHasMax;
	float       m_fMaxVal;
};

class ICvar
{
public:
	void* vtable[10];
};

namespace ConVarStripper
{
	inline void StripAllFlags()
	{
		auto* pCvar = U::Interface.Get<ICvar*>("vstdlib.dll", "VEngineCvar007");

		if (!pCvar)
			return;

		using FindVarFn = ConVar*(__thiscall*)(void*, const char*);

		static const char* s_szUsefulCvars[] = {
			"sv_cheats",
			"mat_wireframe",
			"mat_fullbright",
			"r_drawothermodels",
			"cl_drawshadowtexture",
			"fog_override",
			"fog_enable",
			"r_drawparticles",
			"cl_lagcompensation",
			"net_fakelag",
			"net_fakeloss",
			"host_timescale",
			"sv_infinite_ammo",
			"buddha",
			"thirdperson",
			"cam_idealyaw",
			"cam_idealdist",
			"cam_idealpitch",
		};

		auto fnFindVar = reinterpret_cast<FindVarFn>(
			(*reinterpret_cast<void***>(pCvar))[12]);

		for (const char* szName : s_szUsefulCvars)
		{
			ConVar* pVar = fnFindVar(pCvar, szName);

			if (pVar)
			{
				pVar->m_nFlags &= ~(FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_HIDDEN);
			}
		}
	}
}
