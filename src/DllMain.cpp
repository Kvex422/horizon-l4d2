#include "Entry/Entry.h"
#include "Features/Misc/AntiDetect.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	static bool s_bAttached = false;

	if ((fdwReason == DLL_PROCESS_ATTACH) && !s_bAttached)
	{
		G::ModuleEntry.m_hModule = hinstDLL;
		G::ModuleEntry.Load();

		AntiDetect::ErasePEHeaders(hinstDLL);
		AntiDetect::UnlinkFromPEB(hinstDLL);

		s_bAttached = true;
	}

	return TRUE;
}