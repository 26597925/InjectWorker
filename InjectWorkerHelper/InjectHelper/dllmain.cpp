// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"

#define __DLL_EXPORT extern "C" __declspec(dllexport) 

HWND g_hWinProcess = NULL;
HHOOK g_hHook = NULL;
char g_szDllPath[MAX_PATH] = { 0 };

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


//���ӻص�����
LRESULT CALLBACK HookProc(
	int code,       // hook code
	WPARAM wParam,  // virtual-key code
	LPARAM lParam   // keystroke-message information
	)
{
	//��DLLע��ĳɹ���ʱ�������ټ�����������Ҫע���DLL

	char szDllPath[MAX_PATH] = { 0 };
	BOOL bRet = 0;		
	
	FILE* pFile = fopen("C:\\temp.temp", "r+");
	fread(szDllPath, 1, MAX_PATH, pFile);
	fclose(pFile);
	
	if (!LoadLibrary(szDllPath))
	{
		return 1;
	}
	return CallNextHookEx(NULL, code, wParam, lParam);;
}

__DLL_EXPORT BOOL SetHook(DWORD dwPid, char * szDllPath)
{
	THREADENTRY32 te32 = { 0 };
	HANDLE hThreadSnap = NULL;
	DWORD  dwThreadId = 0;
	
	memcpy(g_szDllPath, szDllPath, MAX_PATH);

	//�����߳̿��ղ���Ŀ��������߳�
	te32.dwSize = sizeof(te32);
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//������ѯĿ��������߳�ID
	OutputDebugString("Thread32First");
	if (Thread32First(hThreadSnap, &te32))
	{
		do
		{
			if (dwPid == te32.th32OwnerProcessID)
			{
				dwThreadId = te32.th32ThreadID;
				break;
			}
		} while (Thread32Next(hThreadSnap, &te32));
	}

	HANDLE hFile = CreateFile("C:\\temp.temp", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		OutputDebugString("������ʱ�ļ�ʧ�ܣ���");
		return FALSE;
	}
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, szDllPath, strlen(szDllPath) + 1, &dwBytesWritten, NULL);
	CloseHandle(hFile);
	hFile = NULL;
	if (dwThreadId != -1)
	{
		//��װ���̹���
		g_hHook = SetWindowsHookExA(WH_KEYBOARD, HookProc, GetModuleHandle(NULL), dwThreadId);
		if (g_hHook)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

__DLL_EXPORT BOOL UnHook()
{
	return UnhookWindowsHookEx(g_hHook);
}

