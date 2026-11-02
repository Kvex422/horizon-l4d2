#pragma once

#include "../Hooks/Hooks.h"

class CGlobal_ModuleEntry
{
public:
	void Load();
	void Unload();

	HMODULE m_hModule = nullptr;
};

namespace G { inline CGlobal_ModuleEntry ModuleEntry; }