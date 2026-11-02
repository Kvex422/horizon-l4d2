#include "TerrorGameRules.h"

using namespace Hooks;

int __fastcall TerrorGameRules::GetSurvivorSet::Detour(void* ecx, void* edx)
{
	return Func.Original<FN>()(ecx, edx);
}

void TerrorGameRules::Init()
{

	{
		using namespace GetSurvivorSet;

		const FN pfGetSurvivorSet = reinterpret_cast<FN>(U::Offsets.m_dwGetSurvivorSet);
		XASSERT(pfGetSurvivorSet == nullptr);

		if (pfGetSurvivorSet)
			XASSERT(Func.Init(pfGetSurvivorSet, &Detour) == false);
	}
}