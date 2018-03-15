// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here

#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <winternl.h>

#include <map>

#if !defined(_UNICODE) && !defined(UNICODE)
#define tstring std::string
#define __MY__TEXT(quote) quote
#else
#define tstring std::wstring
#define __MY__TEXT(quote) L##quote
#endif

#define _MY_TEXT(x)	__MY__TEXT(x)
#define	_MY_T(x)    __MY__TEXT(x)
#define TSTRING     tstring

typedef std::map<unsigned long, unsigned long> UNSIGNEDLONGMAP;
typedef UNSIGNEDLONGMAP::iterator UNSIGNEDLONGMAPIT;
typedef UNSIGNEDLONGMAP::value_type UNSIGNEDLONGMAPPAIR;

#define STATUS_SUCCESS 0x00UL
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004

#define SystemHandleInformation 16
#define SE_DEBUG_PRIVILEGE 0x14

typedef enum _OBJECT_INFORMATION_CLASSEX {
	ObjBasicInformation = 0,
	ObjNameInformation,
	ObjTypeInformation,
} OBJECT_INFORMATION_CLASSEX;

typedef enum _PROCESSINFOCLASSEX
{
	ProcessHandleInformation = 20,
}PROCESSINFOCLASSEX;

typedef struct _SYSTEM_HANDLE
{
	ULONG ProcessId;
	BYTE ObjectTypeNumber;
	BYTE Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantAccess;
}SYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	DWORD HandleCount;
	SYSTEM_HANDLE Handles[1];
}SYSTEM_HANDLE_INFORMATION;

typedef struct _OBJECT_NAME_INFORMATION
{
	UNICODE_STRING ObjectName;
}OBJECT_NAME_INFORMATION;

typedef NTSTATUS(WINAPI *ZwQueryInformationProcessProc)(HANDLE, PROCESSINFOCLASSEX, LPVOID, DWORD, PDWORD);
ZwQueryInformationProcessProc FUN_ZwQueryInformationProcess;

typedef NTSTATUS(WINAPI *ZwQuerySystemInformationProc)(DWORD, PVOID, DWORD, DWORD*);
ZwQuerySystemInformationProc FUN_ZwQuerySystemInformation;

typedef NTSTATUS(WINAPI *ZwQueryObjectProc)(HANDLE, OBJECT_INFORMATION_CLASSEX, PVOID, ULONG, PULONG);
ZwQueryObjectProc FUN_ZwQueryObject;

typedef NTSTATUS(WINAPI *RtlAdjustPrivilegeProc)(DWORD, BOOL, BOOL, PDWORD);
RtlAdjustPrivilegeProc FUN_RtlAdjustPrivilege;

typedef NTSTATUS(WINAPI *ZwGetContextThreadProc)(HANDLE, PCONTEXT);
ZwGetContextThreadProc FUN_ZwGetContextThread;

typedef NTSTATUS(WINAPI *ZwSetContextThreadProc)(HANDLE, PCONTEXT);
ZwSetContextThreadProc FUN_ZwSetContextThread;

typedef DWORD(WINAPI *ZwSuspendThreadProc)(HANDLE, PULONG);
ZwSuspendThreadProc FUN_ZwSuspendThread;

typedef DWORD(WINAPI *ZwResumeThreadProc)(HANDLE, PULONG);
ZwResumeThreadProc FUN_ZwResumeThread;

typedef DWORD(WINAPI *ZwSuspendProcessProc)(HANDLE);
ZwSuspendProcessProc FUN_ZwSuspendProcess;

typedef DWORD(WINAPI *ZwResumeProcessProc)(HANDLE);
ZwResumeProcessProc FUN_ZwResumeProcess;


__inline BOOL ElevatePrivileges()
{
	BOOL bRet = FALSE;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tkp = { 1, {0} };

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

__inline BOOL GetUnDocumentAPI()
{

	FUN_ZwQuerySystemInformation = (ZwQuerySystemInformationProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwQuerySystemInformation");

	FUN_ZwQueryInformationProcess = (ZwQueryInformationProcessProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwQueryInformationProcess");

	FUN_ZwQueryObject = (ZwQueryObjectProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwQueryObject");

	FUN_ZwGetContextThread = (ZwGetContextThreadProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwGetContextThread");

	FUN_ZwSetContextThread = (ZwSetContextThreadProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwSetContextThread");

	FUN_ZwSuspendThread = (ZwSuspendThreadProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwSuspendThread");

	FUN_ZwResumeThread = (ZwResumeThreadProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwResumeThread");

	FUN_ZwSuspendProcess = (ZwSuspendProcessProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwSuspendProcess");

	FUN_ZwResumeProcess = (ZwResumeProcessProc)
		GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "ZwResumeProcess");

	if ((FUN_ZwQuerySystemInformation == NULL) || \
		(FUN_ZwQueryInformationProcess == NULL) || \
		(FUN_ZwQueryObject == NULL) || \
		(FUN_ZwGetContextThread == NULL) || \
		(FUN_ZwSetContextThread == NULL) || \
		(FUN_ZwSuspendThread == NULL) || \
		(FUN_ZwResumeThread == NULL) || \
		(FUN_ZwSuspendProcess == NULL) || \
		(FUN_ZwResumeProcess == NULL))
	{
		return FALSE;
	}

	return TRUE;
}

__inline DWORD GetProcessIdByName(LPCTSTR pName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return 0;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		if (!_tcsicmp(pe.szExeFile, pName))
		{
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	}
	return 0;
}

__inline size_t GetProcessIdByName(UNSIGNEDLONGMAP & ulmap, LPCTSTR pName)
{
	PROCESSENTRY32 pe = { sizeof(pe) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE != hSnapshot)
	{
		for (BOOL fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
		{
			if (!_tcsicmp(pe.szExeFile, pName))
			{
				ulmap.insert(UNSIGNEDLONGMAPPAIR(pe.th32ProcessID, pe.th32ProcessID));
			}
		}

		CloseHandle(hSnapshot);
	}
	return ulmap.size();
}
__inline size_t GetMaxProcessIdByName(UNSIGNEDLONGMAP & ulmap, LPCTSTR pName)
{
	PROCESSENTRY32 pe = { sizeof(pe) };
	DWORD dwProcessID = 0;
	LARGE_INTEGER liCreateTime = { 0 };
	LARGE_INTEGER liCreateTimeTemp = { 0 };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE != hSnapshot)
	{
		for (BOOL fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
		{
			if (!_tcsicmp(pe.szExeFile, pName))
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
				if (hProcess)
				{
					FILETIME ftCreation = { 0 };
					FILETIME ftExit = { 0 };
					FILETIME ftKernel = { 0 };
					FILETIME ftUser = { 0 };
					if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser))
					{
						liCreateTimeTemp.HighPart = ftCreation.dwHighDateTime;
						liCreateTimeTemp.LowPart = ftCreation.dwLowDateTime;
						if (liCreateTime.QuadPart < liCreateTimeTemp.QuadPart)
						{
							dwProcessID = pe.th32ProcessID;
							memcpy(&liCreateTime, &liCreateTimeTemp, sizeof(LARGE_INTEGER));
						}
					}
					CloseHandle(hProcess);
				}
			}
		}
		CloseHandle(hSnapshot);

		if (dwProcessID > 0)
		{
			ulmap.insert(UNSIGNEDLONGMAPPAIR(dwProcessID, dwProcessID));
		}
	}
	return ulmap.size();
}


//��ȡ�����ļ�·��
__inline tstring GetProgramPath()
{
	tstring tstrFilePath = _T("");
	_TCHAR tFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, tFilePath, MAX_PATH);
	if (*tFilePath)
	{
		_TCHAR * pFound = _tcsrchr(tFilePath, _T('\\'));
		if (*(++pFound))
		{
			*pFound = _T('\0');
		}
		tstrFilePath = tFilePath;
	}
	return tstrFilePath;
}

//���ݽ���ID��ֹ����
__inline void TerminateProcessByProcessId(DWORD dwProcessId)
{
	DWORD dwExitCode = 0;
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
	if (hProcess)
	{
		GetExitCodeProcess(hProcess, &dwExitCode);
		TerminateProcess(hProcess, dwExitCode);
		CloseHandle(hProcess);
		hProcess = 0;
	}
}

//����Ӧ�ó����ļ����ơ��������������ͼ��ȴ�ʱ����������
typedef enum LaunchType {
	LTYPE_0 = 0, //����
	LTYPE_1 = 1, //ֱ��
	LTYPE_2 = 2, //�ӳ�(�趨�ȴ�ʱ��)
}LAUNCHTYPE;

//����Ӧ�ó����ļ����ơ��������������ͼ��ȴ�ʱ����������
__inline  BOOL LaunchAppProg(tstring tsAppProgName, tstring tsArguments = _T(""), bool bNoUI = true, LAUNCHTYPE type = LTYPE_0, DWORD dwWaitTime = WAIT_TIMEOUT)
{
	BOOL bRet = FALSE;
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DWORD dwCreateFlags = CREATE_NO_WINDOW;
	LPTSTR lpArguments = NULL;

	if (tsArguments.length())
	{
		lpArguments = (LPTSTR)tsArguments.c_str();
	}

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!bNoUI)
	{
		dwCreateFlags = 0;
	}

	// Start the child process.
	bRet = CreateProcess(tsAppProgName.c_str(),   // No module name (use command line)
		lpArguments,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		dwCreateFlags,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi);           // Pointer to PROCESS_INFORMATION structure
	if (bRet)
	{
		switch (type)
		{
		case LTYPE_0:
		{
			// No wait until child process exits.
		}
		break;
		case LTYPE_1:
		{
			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, INFINITE);
		}
		break;
		case LTYPE_2:
		{
			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, dwWaitTime);
		}
		break;
		default:
			break;
		}

		// Close process and thread handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// Exit process.
		//TerminateProcessByProcessId(pi.dwProcessId);
	}
	else
	{
		//DEBUG_TRACE(_T("CreateProcess failed (%d).\n"), GetLastError());
	}
	return bRet;
}

//����Ӧ�ó����ļ����ơ��������������ͼ��ȴ�ʱ����������
__inline DWORD LaunchProgram(tstring tsAppProgName, tstring tsArguments = _T(""), DWORD dwFlags = CREATE_NEW_CONSOLE, LPVOID lpEnvironment = NULL, LPCTSTR lpCurrentDirectory = NULL, LAUNCHTYPE type = LTYPE_0, DWORD dwWaitTime = WAIT_TIMEOUT)
{
	BOOL bRet = FALSE;
	DWORD dwRet = (-1L);
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DWORD dwCreateFlags = dwFlags;
	LPTSTR lpArguments = NULL;

	if (tsArguments.length())
	{
		lpArguments = (LPTSTR)tsArguments.c_str();
	}

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process.
	bRet = CreateProcess(tsAppProgName.c_str(),   // No module name (use command line)
		lpArguments,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		dwCreateFlags,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi);           // Pointer to PROCESS_INFORMATION structure
	if (bRet)
	{
		switch (type)
		{
		case LTYPE_0:
		{
			// No wait until child process exits.
		}
		break;
		case LTYPE_1:
		{
			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, INFINITE);
		}
		break;
		case LTYPE_2:
		{
			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, dwWaitTime);
		}
		break;
		default:
			break;
		}
		dwRet = pi.dwProcessId;
		// Close process and thread handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// Exit process.
		//TerminateProcessByProcessId(pi.dwProcessId);
	}
	else
	{
		//DEBUG_TRACE(_T("CreateProcess failed (%d).\n"), GetLastError());
	}
	return dwRet;
}

//����Ӧ�ó����ļ����ơ��������������ͼ��ȴ�ʱ����������
__inline BOOL StartupProgram(tstring tsAppProgName, tstring tsArguments = _T(""), STARTUPINFO * pStartupInfo = NULL, PROCESS_INFORMATION * pProcessInformation = NULL, DWORD dwFlags = CREATE_NEW_CONSOLE, LPVOID lpEnvironment = NULL, LPCTSTR lpCurrentDirectory = NULL, LAUNCHTYPE type = LTYPE_0, DWORD dwWaitTime = WAIT_TIMEOUT)
{
	BOOL bRet = FALSE;
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DWORD dwCreateFlags = dwFlags;
	LPTSTR lpArguments = NULL;
	STARTUPINFO * pSI = &si;
	PROCESS_INFORMATION * pPI = &pi;

	if (pStartupInfo)
	{
		pSI = pStartupInfo;
	}
	if (pProcessInformation)
	{
		pPI = pProcessInformation;
	}

	if (tsArguments.length())
	{
		lpArguments = (LPTSTR)tsArguments.c_str();
	}

	pSI->cb = sizeof(STARTUPINFO);

	// Start the child process.
	bRet = CreateProcess(tsAppProgName.c_str(),   // No module name (use command line)
		lpArguments,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		dwCreateFlags,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		pSI,            // Pointer to STARTUPINFO structure
		pPI);           // Pointer to PROCESS_INFORMATION structure
	if (bRet)
	{
		switch (type)
		{
		case LTYPE_0:
		{
			// No wait until child process exits.
		}
		break;
		case LTYPE_1:
		{
			// Wait until child process exits.
			WaitForSingleObject(pPI->hProcess, INFINITE);
		}
		break;
		case LTYPE_2:
		{
			// Wait until child process exits.
			WaitForSingleObject(pPI->hProcess, dwWaitTime);
		}
		break;
		default:
			break;
		}

		// Close process and thread handles.
		//CloseHandle(pPI->hProcess);
		//CloseHandle(pPI->hThread);

		// Exit process.
		//TerminateProcessByProcessId(pPI->dwProcessId);
	}
	else
	{
		//DEBUG_TRACE(_T("CreateProcess failed (%d).\n"), GetLastError());
	}
	return bRet;
}

//	ANSI to Unicode
__inline static std::wstring ANSIToUnicode(const std::string& str)
{
	int len = 0;
	len = str.length();
	int unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t * pUnicode;
	pUnicode = new  wchar_t[(unicodeLen + 1)];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	std::wstring rt;
	rt = (wchar_t*)pUnicode;
	delete pUnicode;
	return rt;
}

//Unicode to ANSI
__inline static std::string UnicodeToANSI(const std::wstring& str)
{
	char* pElementText;
	int iTextLen;
	iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	std::string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}
//UTF - 8 to Unicode
__inline static std::wstring UTF8ToUnicode(const std::string& str)
{
	int len = 0;
	len = str.length();
	int unicodeLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t * pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	std::wstring rt;
	rt = (wchar_t*)pUnicode;
	delete pUnicode;
	return rt;
}
//Unicode to UTF - 8
__inline static std::string UnicodeToUTF8(const std::wstring& str)
{
	char*   pElementText;
	int iTextLen;
	iTextLen = WideCharToMultiByte(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
	::WideCharToMultiByte(CP_UTF8,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	std::string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

__inline static std::string TToA(tstring tsT)
{
	std::string str = "";

#if !defined(UNICODE) && !defined(_UNICODE)
	str = tsT;
#else
	str = UnicodeToANSI(tsT);
#endif

	return str;
}

__inline static std::wstring TToW(tstring tsT)
{
	std::wstring wstr = L"";

#if !defined(UNICODE) && !defined(_UNICODE)
	wstr = ANSIToUnicode(tsT);
#else
	wstr = tsT;
#endif

	return wstr;
}

__inline static tstring AToT(std::string str)
{
	tstring ts = _T("");

#if !defined(UNICODE) && !defined(_UNICODE)
	ts = str;
#else
	ts = ANSIToUnicode(str);
#endif

	return ts;
}

__inline static tstring WToT(std::wstring wstr)
{
	tstring ts = _T("");

#if !defined(UNICODE) && !defined(_UNICODE)
	ts = UnicodeToANSI(wstr);
#else
	ts = wstr;
#endif

	return ts;
}

__inline int t_main(int argc, char* argv[])
{
	HANDLE duplicateHnd = 0;
	HANDLE sourceHnd = 0;
	SIZE_T procHndNum = 0;
	ULONG pidCount = 0;
	SYSTEM_HANDLE* currnetHnd = 0;
	DWORD buffLen = sizeof(SYSTEM_HANDLE_INFORMATION);
	NTSTATUS status = 0;
	SYSTEM_HANDLE_INFORMATION* buff = (SYSTEM_HANDLE_INFORMATION*)malloc(buffLen);
	_TCHAR tFileName[MAX_PATH] = _T("WeChat.exe");
	WCHAR wTypeObjName[MAX_PATH] = L"Mutant";
	WCHAR *wMutexList[] = {
		L"_WeChat_App_Instance_Identity_Mutex_Name",
		L"WeChat_GlobalConfig_Multi_Process_Mutex",
	};
	INT nMutexNum = sizeof(wMutexList) / sizeof(WCHAR *);
	INT nMutexIdx = 0;
	OBJECT_NAME_INFORMATION* objNameInfo = 0;
	OBJECT_NAME_INFORMATION* objTypeInfo = 0;
	UNSIGNEDLONGMAP ulmap;

	pidCount = GetMaxProcessIdByName(ulmap, tFileName);
	if (pidCount <= 0)
	{
		goto __LEAVE_CLEAN__;
	}

	if ((ElevatePrivileges() == FALSE) || (GetUnDocumentAPI() == FALSE))
	{
		goto __LEAVE_CLEAN__;
	}

	objNameInfo = (OBJECT_NAME_INFORMATION*)malloc(0x1000);
	objTypeInfo = (OBJECT_NAME_INFORMATION*)malloc(0x1000);

	sourceHnd = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_DUP_HANDLE | PROCESS_SUSPEND_RESUME, FALSE, ulmap.begin()->first);
	(FUN_ZwSuspendProcess)(sourceHnd);
	(FUN_ZwQueryInformationProcess)(sourceHnd, ProcessHandleInformation, &procHndNum, sizeof(SIZE_T), NULL);
	//������Ч�����sizeof(SIZE_T)��ʼ,ÿ����sizeof(SIZE_T)����
	for (int idx = 0, hndNum = sizeof(SIZE_T); idx < procHndNum && hndNum < 16384; hndNum += sizeof(SIZE_T))
	{
		//�ж��Ƿ�Ϊ��Ч���������TRUE��������Ч���
		if (!DuplicateHandle(sourceHnd,
			(HANDLE)hndNum,
			GetCurrentProcess(),
			&duplicateHnd, 0, FALSE, DUPLICATE_SAME_ACCESS))
		{
			continue;
		}
		else
		{
			memset(objNameInfo, 0, 0x1000);
			memset(objTypeInfo, 0, 0x1000);

			FUN_ZwQueryObject((HANDLE)duplicateHnd, ObjNameInformation, objNameInfo, 0x1000, NULL);
			FUN_ZwQueryObject((HANDLE)duplicateHnd, ObjTypeInformation, objTypeInfo, 0x1000, NULL);

			//�ҵ������� �Ƚ�����
			if (_wcsicmp(objTypeInfo->ObjectName.Buffer, wTypeObjName) == 0)
			{
				if (objNameInfo->ObjectName.Buffer)
				{
					for (int n = 0; n < nMutexNum; n++)
					{
						if (wcsstr(objNameInfo->ObjectName.Buffer, wMutexList[n]) != 0)
						{
							CloseHandle(duplicateHnd);

							if (DuplicateHandle(sourceHnd,
								(HANDLE)hndNum,
								GetCurrentProcess(),
								&duplicateHnd, 0, FALSE, DUPLICATE_CLOSE_SOURCE))
							{
								nMutexIdx++;
								break;
							}
						}
					}
				}
			}
			CloseHandle(duplicateHnd);
			idx++;

			if (nMutexIdx >= nMutexNum)
			{
				nMutexIdx = 0;
				break;
			}
		}
	}
	(FUN_ZwResumeProcess)(sourceHnd);

__LEAVE_CLEAN__:

	if (objTypeInfo)
	{
		free(objTypeInfo);
		objTypeInfo = 0;
	}
	if (objNameInfo)
	{
		free(objNameInfo);
		objNameInfo = 0;
	}

	return 0;
}


//���̿��գ�ö�ٸ����̣�
__inline BOOL GetPidByProcessName(LPCTSTR lpszProcessName, SIZE_T &stPid)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return FALSE;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
	{
		::CloseHandle(hSnapshot);
		return FALSE;
	}

	while (Process32Next(hSnapshot, &pe))
	{
		if (!_tcsicmp(lpszProcessName, pe.szExeFile))
		{
			::CloseHandle(hSnapshot);
			stPid = pe.th32ProcessID;
			return TRUE;
		}
	}

	::CloseHandle(hSnapshot);

	return FALSE;
}

/********************************************************************************************************/

//ע��DLL��Զ�̽���
__inline BOOL InjectDllToRemoteProcess(const _TCHAR* lpDllName, const _TCHAR* lpPid, const _TCHAR* lpProcName)
{
	SIZE_T stPid = 0;

	if ((ElevatePrivileges() == FALSE) || (GetUnDocumentAPI() == FALSE))
	{
		return FALSE;
	}

	if (_tcsstr(lpProcName, _T("\\")) || _tcsstr(lpProcName, _T("/")))
	{
		//stPid = LaunchProgram(lpProcName, _T(""), CREATE_SUSPENDED);
		stPid = LaunchProgram(lpProcName, _T(""));
	}

	if (stPid <= 0)
	{
		if (NULL == lpPid || 0 == _tcslen(lpPid))
		{
			if (NULL != lpProcName && 0 != _tcslen(lpProcName))
			{
				if (!GetPidByProcessName(lpProcName, stPid))
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			stPid = _ttoi(lpPid);
		}
	}

	//����Pid�õ����̾��(ע�����Ȩ��)
	HANDLE hRemoteProcess = NULL;
	hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, stPid);
	//hRemoteProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME, FALSE, stPid);
	if (!hRemoteProcess && INVALID_HANDLE_VALUE == hRemoteProcess)
	{
		return FALSE;
	}
	
	//�������
	(FUN_ZwSuspendProcess)(hRemoteProcess);

	//����DLL·������Ҫ���ڴ�ռ�
	SIZE_T stSize = (1 + _tcslen(lpDllName)) * sizeof(_TCHAR);

	//ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ���������,�ɹ����ط����ڴ���׵�ַ.
	LPVOID lpRemoteBuff = VirtualAllocEx(hRemoteProcess, NULL, stSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (NULL == lpRemoteBuff)
	{
		CloseHandle(hRemoteProcess);
		return FALSE;
	}
	
	//ʹ��WriteProcessMemory������DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ�,�ɹ�����TRUE.
	SIZE_T stHasWrite = 0;
	BOOL bRet = WriteProcessMemory(hRemoteProcess, lpRemoteBuff, lpDllName, stSize, &stHasWrite);
	if (!bRet || stHasWrite != stSize)
	{
		VirtualFreeEx(hRemoteProcess, lpRemoteBuff, stSize, MEM_DECOMMIT | MEM_RELEASE);
		CloseHandle(hRemoteProcess);
		return FALSE;
	}

	//����һ�����������̵�ַ�ռ������е��߳�(Ҳ��:����Զ���߳�),�ɹ��������߳̾��.
	//ע��:���̾������߱�PROCESS_CREATE_THREAD, PROCESS_QUERY_INFORMATION, PROCESS_VM_OPERATION, PROCESS_VM_WRITE,��PROCESS_VM_READ����Ȩ��
	DWORD dwRemoteThread = 0;
	LPTHREAD_START_ROUTINE pfnLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")), "LoadLibraryA");
	HANDLE hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnLoadLibrary, lpRemoteBuff, 0, &dwRemoteThread);
	if (INVALID_HANDLE_VALUE == hRemoteThread)
	{
		VirtualFreeEx(hRemoteProcess, lpRemoteBuff, stSize, MEM_DECOMMIT | MEM_RELEASE);
		CloseHandle(hRemoteProcess);
		return FALSE;
	}

	//ע��ɹ��ͷž��
	WaitForSingleObject(hRemoteThread, INFINITE);

	(FUN_ZwResumeProcess)(hRemoteProcess);

	CloseHandle(hRemoteThread);
	CloseHandle(hRemoteProcess);
	
	return TRUE;
}

__inline BOOL InjectDll(DWORD dwProcessId, DWORD dwThreadId, const _TCHAR * ptzDllName)
{
	BOOL bResult = FALSE;
	FARPROC farproc = NULL;
	CONTEXT context = { 0 };
	LPVOID lpCodeBase = NULL;
	SIZE_T stCodeSize = USN_PAGE_SIZE;
	SIZE_T stNumberOfBytesWritten = 0;
	DWORD dwCurrentEipAddress = 0;
	DWORD dwIndexOffsetPosition = 0;
	CHAR szCodeData[USN_PAGE_SIZE] = { 0 };
	CHAR szCode0[] = ("\x60\xE8\x00\x00\x00\x00\x58\x83\xC0\x13\x50\xB8");
	CHAR szCode1[] = ("\xFF\xD0\x61\x68");
	CHAR szCode2[] = ("\xC3");

	//����Pid�õ����̾��(ע�����Ȩ��)
	HANDLE hRemoteProcess = NULL;
	HANDLE hRemoteThread = NULL;
	ULONG ulPreviousSuspendCount = 0;
	if ((ElevatePrivileges() == FALSE) || (GetUnDocumentAPI() == FALSE))
	{
		return FALSE;
	}

	hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	//hRemoteProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_SUSPEND_RESUME, FALSE, stPid);
	if (!hRemoteProcess && INVALID_HANDLE_VALUE == hRemoteProcess)
	{
		return FALSE;
	}

	hRemoteThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);
	//hRemoteProcess = OpenThread(THREAD_TERMINATE | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SET_INFORMATION |THREAD_SET_THREAD_TOKEN | THREAD_IMPERSONATE | THREAD_DIRECT_IMPERSONATION | THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, stPid);
	if (!hRemoteThread && INVALID_HANDLE_VALUE == hRemoteThread)
	{
		return FALSE;
	}

	//�������
	(FUN_ZwSuspendProcess)(hRemoteProcess);

	//�����߳�
	(FUN_ZwSuspendThread)(hRemoteThread, &ulPreviousSuspendCount);
	
	//��Զ�̽��̷����ִ�ж�дģ��
	lpCodeBase = VirtualAllocEx(hRemoteProcess, lpCodeBase, stCodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	
	//�����߳������ĵı�ʶ
	context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS;
	
	//��ȡ�߳�������
	(FUN_ZwGetContextThread)(hRemoteThread, &context);

	//��ȡԶ�̽��̵ĵ�ǰִ�е�ַ
	dwCurrentEipAddress = context.Eip;

	//����Զ�̽��̵���һִ�е�ַ
	context.Eip = (DWORD)lpCodeBase;

	//��ȡLoadLibraryA�ĺ�����ַ
	farproc = GetProcAddress(GetModuleHandle(_T("KERNEL32.DLL")), ("LoadLibraryA"));

	///////////////////////////////////////////////////////////////////////////////////
	// ���ݿ�
	// CodeData �� 
	//		{ 96, 232, 0, 0, 0, 0, 88, 131, 192, 19, 80, 184 } / "\x60\xE8\x00\x00\x00\x00\x58\x83\xC0\x13\x50\xB8" 
	//		{ LoadLibraryA������ַ }
	//		{ 255, 208, 97, 104 } / "\xFF\xD0\x61\x68"
	//		{ Eip��ַ }
	//		{ 195 } / "\xC3"
	//		{ Dll·�� };
	
	memcpy(szCodeData + dwIndexOffsetPosition, szCode0, sizeof(szCode0) - 1);
	dwIndexOffsetPosition += sizeof(szCode0) - 1;
	memcpy(szCodeData + dwIndexOffsetPosition, &farproc, sizeof(farproc));
	dwIndexOffsetPosition += sizeof(farproc);
	memcpy(szCodeData + dwIndexOffsetPosition, szCode1, sizeof(szCode1) - 1);
	dwIndexOffsetPosition += sizeof(szCode1) - 1;
	memcpy(szCodeData + dwIndexOffsetPosition, &dwCurrentEipAddress, sizeof(dwCurrentEipAddress));
	dwIndexOffsetPosition += sizeof(dwCurrentEipAddress);
	memcpy(szCodeData + dwIndexOffsetPosition, szCode2, sizeof(szCode2) - 1);
	dwIndexOffsetPosition += sizeof(szCode2) - 1;
	memcpy(szCodeData + dwIndexOffsetPosition, TToA(ptzDllName).c_str(), TToA(ptzDllName).length());
	dwIndexOffsetPosition += TToA(ptzDllName).length();
	
	//д��Զ�̽��̿�ִ�ж�дģ��
	WriteProcessMemory(hRemoteProcess, lpCodeBase, szCodeData, dwIndexOffsetPosition, &stNumberOfBytesWritten);
	
	//�����߳�������
	(FUN_ZwSetContextThread)(hRemoteThread, &context);

	//�ָ��߳�
	(FUN_ZwResumeThread)(hRemoteThread, &ulPreviousSuspendCount);

	//�ָ�����
	(FUN_ZwResumeProcess)(hRemoteProcess);

	//�ͷ���Զ�̽��̷���Ŀ�ִ�ж�дģ��
	//VirtualFreeEx(hRemoteProcess, lpCodeBase, stCodeSize, MEM_DECOMMIT | MEM_RELEASE);

	return bResult;
}