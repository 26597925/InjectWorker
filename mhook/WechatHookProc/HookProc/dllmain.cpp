// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "function.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	VOIDVOIDMAPIT itVVM;
	VOIDVOIDMAPIT itVVMEnd;
	STRINGVOIDVOIDMAPIT itSVVM;
	STRINGVOIDVOIDMAPIT itSVVMEnd;
	STRINGSTRINGVOIDVOIDMAPIT itSSVVM;
	STRINGSTRINGVOIDVOIDMAPIT itSSVVMEnd;
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		//����LOG4Z  
		//û������κ���־��¼��������� LOG4Z��Ĭ�Ϲ���һ��.  
		//ILog4zManager::getInstance()->start();

		InitDebugConsole();
		DebugPrintString(_T("Enter DLL_PROCESS_ATTACH!"));
		itSSVVM = G_SSVVM.begin();
		itSSVVMEnd = G_SSVVM.end();
		for (; itSSVVM != itSSVVMEnd; itSSVVM++)
		{
			DebugPrintString(tstring(_T("Enter module ")) + AToT(itSSVVM->first));
			itSVVM = itSSVVM->second.begin();
			itSVVMEnd = itSSVVM->second.end();
			for (; itSVVM != itSVVMEnd; itSVVM++)
			{
				itVVM = itSVVM->second.begin();
				itVVMEnd = itSVVM->second.end();
				DebugPrintString(tstring(_T("Hook function ")) + AToT(itSVVM->first) + _T(",TrueAddr=") + STRING_FROM_HEX((unsigned long)itVVM->first) + _T(",HookAddr=") + STRING_FROM_HEX((unsigned long)itVVM->second));
				
				Mhook_SetHook((PVOID*)itVVM->first, itVVM->second);
			}
		}
	}
	break;
	case DLL_THREAD_ATTACH:
	{

	}
	break;
	case DLL_THREAD_DETACH:
	{

	}
	break;
	case DLL_PROCESS_DETACH:
	{
		DebugPrintString(_T("Enter DLL_PROCESS_DETACH!"));
		itSSVVM = G_SSVVM.begin();
		itSSVVMEnd = G_SSVVM.end();
		for (; itSSVVM != itSSVVMEnd; itSSVVM++)
		{
			DebugPrintString(tstring(_T("Enter module ")) + AToT(itSSVVM->first));
			itSVVM = itSSVVM->second.begin();
			itSVVMEnd = itSSVVM->second.end();
			for (; itSVVM != itSVVMEnd; itSVVM++)
			{
				itVVM = itSVVM->second.begin();
				itVVMEnd = itSVVM->second.end();
				DebugPrintString(tstring(_T("Remove Hook function ")) + AToT(itSVVM->first) + _T(",TrueAddr=") + STRING_FROM_HEX((unsigned long)itVVM->first) + _T(",HookAddr=") + STRING_FROM_HEX((unsigned long)itVVM->second));
				// Remove the hook
				Mhook_Unhook((PVOID*)itVVM->first);
			}
		}

		ExitDebugConsole();
	}
	break;
	}
	return TRUE;
}

