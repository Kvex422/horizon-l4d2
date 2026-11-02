#pragma once

#include <Windows.h>
#include <winternl.h>

namespace AntiDetect
{
	inline void ErasePEHeaders(HMODULE hModule)
	{
		PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
		PIMAGE_NT_HEADERS pNtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(
			reinterpret_cast<BYTE*>(hModule) + pDosHeader->e_lfanew);

		DWORD dwOldProtect = 0;
		const DWORD dwHeaderSize = pNtHeaders->OptionalHeader.SizeOfHeaders;

		if (VirtualProtect(hModule, dwHeaderSize, PAGE_READWRITE, &dwOldProtect))
		{
			memset(hModule, 0, dwHeaderSize);
			VirtualProtect(hModule, dwHeaderSize, dwOldProtect, &dwOldProtect);
		}
	}

	inline void UnlinkFromPEB(HMODULE hModule)
	{
#ifdef _WIN32
		PPEB pPeb = reinterpret_cast<PPEB>(__readfsdword(0x30));
		PLIST_ENTRY pHead = &pPeb->Ldr->InMemoryOrderModuleList;
		PLIST_ENTRY pCurrent = pHead->Flink;

		while (pCurrent != pHead)
		{
			PLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pCurrent, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			if (pEntry->DllBase == hModule)
			{
				pCurrent->Blink->Flink = pCurrent->Flink;
				pCurrent->Flink->Blink = pCurrent->Blink;
				break;
			}

			pCurrent = pCurrent->Flink;
		}
#endif
	}
}
