
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

#include "codeconv.h"

namespace UNK_API{
	typedef enum _PROCESSINFOCLASSEX
	{
		ProcessHandleInformation = 20,
	}PROCESSINFOCLASSEX;
	typedef LONG NTSTATUS;

#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)   
#define STATUS_UNSUCCESSFUL             ((NTSTATUS)0xC0000001L)   
#define STATUS_NOT_IMPLEMENTED          ((NTSTATUS)0xC0000002L)   
#define STATUS_INVALID_INFO_CLASS       ((NTSTATUS)0xC0000003L)   
#define STATUS_INFO_LENGTH_MISMATCH     ((NTSTATUS)0xC0000004L)   

	typedef enum _SYSTEM_INFORMATION_CLASS
	{
		SystemBasicInformation,                 //  0 Y N   
		SystemProcessorInformation,             //  1 Y N   
		SystemPerformanceInformation,           //  2 Y N   
		SystemTimeOfDayInformation,             //  3 Y N   
		SystemNotImplemented1,                  //  4 Y N   
		SystemProcessesAndThreadsInformation,   //  5 Y N   
		SystemCallCounts,                       //  6 Y N   
		SystemConfigurationInformation,         //  7 Y N   
		SystemProcessorTimes,                   //  8 Y N   
		SystemGlobalFlag,                       //  9 Y Y   
		SystemNotImplemented2,                  // 10 Y N   
		SystemModuleInformation,                // 11 Y N   
		SystemLockInformation,                  // 12 Y N   
		SystemNotImplemented3,                  // 13 Y N   
		SystemNotImplemented4,                  // 14 Y N   
		SystemNotImplemented5,                  // 15 Y N   
		SystemHandleInformation,                // 16 Y N   
		SystemObjectInformation,                // 17 Y N   
		SystemPagefileInformation,              // 18 Y N   
		SystemInstructionEmulationCounts,       // 19 Y N   
		SystemInvalidInfoClass1,                // 20   
		SystemCacheInformation,                 // 21 Y Y   
		SystemPoolTagInformation,               // 22 Y N   
		SystemProcessorStatistics,              // 23 Y N   
		SystemDpcInformation,                   // 24 Y Y   
		SystemNotImplemented6,                  // 25 Y N   
		SystemLoadImage,                        // 26 N Y   
		SystemUnloadImage,                      // 27 N Y   
		SystemTimeAdjustment,                   // 28 Y Y   
		SystemNotImplemented7,                  // 29 Y N   
		SystemNotImplemented8,                  // 30 Y N   
		SystemNotImplemented9,                  // 31 Y N   
		SystemCrashDumpInformation,             // 32 Y N   
		SystemExceptionInformation,             // 33 Y N   
		SystemCrashDumpStateInformation,        // 34 Y Y/N   
		SystemKernelDebuggerInformation,        // 35 Y N   
		SystemContextSwitchInformation,         // 36 Y N   
		SystemRegistryQuotaInformation,         // 37 Y Y   
		SystemLoadAndCallImage,                 // 38 N Y   
		SystemPrioritySeparation,               // 39 N Y   
		SystemNotImplemented10,                 // 40 Y N   
		SystemNotImplemented11,                 // 41 Y N   
		SystemInvalidInfoClass2,                // 42   
		SystemInvalidInfoClass3,                // 43   
		SystemTimeZoneInformation,              // 44 Y N   
		SystemLookasideInformation,             // 45 Y N   
		SystemSetTimeSlipEvent,                 // 46 N Y   
		SystemCreateSession,                    // 47 N Y   
		SystemDeleteSession,                    // 48 N Y   
		SystemInvalidInfoClass4,                // 49   
		SystemRangeStartInformation,            // 50 Y N   
		SystemVerifierInformation,              // 51 Y Y   
		SystemAddVerifier,                      // 52 N Y   
		SystemSessionProcessesInformation       // 53 Y N   

	} SYSTEM_INFORMATION_CLASS;

	typedef struct _LSA_UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;

	} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

	typedef struct _CLIENT_ID
	{
		HANDLE UniqueProcess;
		HANDLE UniqueThread;

	} CLIENT_ID;

	typedef enum _THREAD_STATE
	{
		StateInitialized,
		StateReady,
		StateRunning,
		StateStandby,
		StateTerminated,
		StateWait,
		StateTransition,
		StateUnknown

	} THREAD_STATE;

	typedef enum _KWAIT_REASON
	{
		Executive,
		FreePage,
		PageIn,
		PoolAllocation,
		DelayExecution,
		Suspended,
		UserRequest,
		WrExecutive,
		WrFreePage,
		WrPageIn,
		WrPoolAllocation,
		WrDelayExecution,
		WrSuspended,
		WrUserRequest,
		WrEventPair,
		WrQueue,
		WrLpcReceive,
		WrLpcReply,
		WrVirtualMemory,
		WrPageOut,
		WrRendezvous,
		Spare2,
		Spare3,
		Spare4,
		Spare5,
		Spare6,
		WrKernel

	} KWAIT_REASON;

	/*typedef struct _IO_COUNTERS
	{
	LARGE_INTEGER ReadOperationCount;   //I/O��������Ŀ
	LARGE_INTEGER WriteOperationCount;  //I/Oд������Ŀ
	LARGE_INTEGER OtherOperationCount;  //I/O����������Ŀ
	LARGE_INTEGER ReadTransferCount;    //I/O��������Ŀ
	LARGE_INTEGER WriteTransferCount;   //I/Oд������Ŀ
	LARGE_INTEGER OtherTransferCount;   //I/O��������������Ŀ

	} IO_COUNTERS, *PIO_COUNTERS;
	*/
	typedef struct _VM_COUNTERS
	{
		ULONG PeakVirtualSize;              //����洢��ֵ��С   
		ULONG VirtualSize;                  //����洢��С   
		ULONG PageFaultCount;               //ҳ������Ŀ   
		ULONG PeakWorkingSetSize;           //��������ֵ��С   
		ULONG WorkingSetSize;               //��������С   
		ULONG QuotaPeakPagedPoolUsage;      //��ҳ��ʹ������ֵ   
		ULONG QuotaPagedPoolUsage;          //��ҳ��ʹ�����   
		ULONG QuotaPeakNonPagedPoolUsage;   //�Ƿ�ҳ��ʹ������ֵ   
		ULONG QuotaNonPagedPoolUsage;       //�Ƿ�ҳ��ʹ�����   
		ULONG PagefileUsage;                //ҳ�ļ�ʹ�����   
		ULONG PeakPagefileUsage;            //ҳ�ļ�ʹ�÷�ֵ   

	} VM_COUNTERS, *PVM_COUNTERS;

	typedef LONG KPRIORITY;

	typedef struct _SYSTEM_THREADS32
	{
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER CreateTime;
		ULONG WaitTime;
		PVOID StartAddress;
		CLIENT_ID ClientId;
		KPRIORITY Priority;
		KPRIORITY BasePriority;
		ULONG ContextSwitchCount;
		THREAD_STATE State;
		KWAIT_REASON WaitReason;
	} SYSTEM_THREADS32, *PSYSTEM_THREADS32;

	typedef struct _SYSTEM_THREADS64
	{
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER CreateTime;
		ULONG WaitTime;
		PVOID StartAddress;
		CLIENT_ID ClientId;
		KPRIORITY Priority;
		KPRIORITY BasePriority;
		ULONG ContextSwitchCount;
		THREAD_STATE State;
		KWAIT_REASON WaitReason;
		ULONG Reserved;
	} SYSTEM_THREADS64, *PSYSTEM_THREADS64;

	typedef struct _SYSTEM_PROCESSES32
	{
		ULONG NextEntryDelta;
		ULONG ThreadCount;
		ULONG Reserved1[6];
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ProcessName;
		KPRIORITY BasePriority;
		ULONG ProcessId;
		ULONG InheritedFromProcessId;
		ULONG HandleCount;
		ULONG Reserved2[2];
		VM_COUNTERS VmCounters;
		IO_COUNTERS IoCounters;
		SYSTEM_THREADS32 Threads[1];
	} SYSTEM_PROCESSES32, *PSYSTEM_PROCESSES32;

	typedef struct _SYSTEM_PROCESSES64
	{
		ULONG NextEntryDelta;
		ULONG ThreadCount;
		ULONG Reserved1[6];
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ProcessName;
		KPRIORITY BasePriority;
		HANDLE ProcessId;
		HANDLE InheritedFromProcessId;
		ULONG HandleCount;
		ULONG Reserved2[2];
		SIZE_T PageDirectoryBase;
		VM_COUNTERS VmCounters;
		SIZE_T PrivatePageCount;
		IO_COUNTERS IoCounters;
		SYSTEM_THREADS64 Threads[1];
	} SYSTEM_PROCESSES64, *PSYSTEM_PROCESSES64;

	typedef struct _SYSTEM_BASIC_INFORMATION
	{
		BYTE Reserved1[24];
		PVOID Reserved2[4];
		CCHAR NumberOfProcessors;

	} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

	typedef struct _SYSTEM_MODULE_INFORMATION32 {
		ULONG Reserved[2];
		PVOID Base;
		ULONG Size;
		ULONG Flags;
		USHORT Index;
		USHORT Unknown;
		USHORT LoadCount;
		USHORT ModuleNameOffset;
		CHAR ImageName[256];
	} SYSTEM_MODULE_INFORMATION32, *PSYSTEM_MODULE_INFORMATION32;

	typedef struct _SYSTEM_MODULE_INFORMATION64 {
		ULONG Reserved[3];
		PVOID Base;
		ULONG Size;
		ULONG Flags;
		USHORT Index;
		USHORT Unknown;
		USHORT LoadCount;
		USHORT ModuleNameOffset;
		CHAR ImageName[256];
	} SYSTEM_MODULE_INFORMATION64, *PSYSTEM_MODULE_INFORMATION64;

#if !defined(_WIN64) && !defined(WIN64)
	typedef _SYSTEM_MODULE_INFORMATION32 _SYSTEM_MODULE_INFORMATION;
	typedef SYSTEM_MODULE_INFORMATION32 SYSTEM_MODULE_INFORMATION;
	typedef PSYSTEM_MODULE_INFORMATION32 PSYSTEM_MODULE_INFORMATION;

	typedef _SYSTEM_THREADS32 _SYSTEM_THREADS;
	typedef SYSTEM_THREADS32 SYSTEM_THREADS;
	typedef PSYSTEM_THREADS32 PSYSTEM_THREADS;

	typedef _SYSTEM_PROCESSES32 _SYSTEM_PROCESSES;
	typedef SYSTEM_PROCESSES32 SYSTEM_PROCESSES;
	typedef PSYSTEM_PROCESSES32 PSYSTEM_PROCESSES;
#else
	typedef _SYSTEM_MODULE_INFORMATION64 _SYSTEM_MODULE_INFORMATION;
	typedef SYSTEM_MODULE_INFORMATION64 SYSTEM_MODULE_INFORMATION;
	typedef PSYSTEM_MODULE_INFORMATION64 PSYSTEM_MODULE_INFORMATION;

	typedef _SYSTEM_THREADS64 _SYSTEM_THREADS;
	typedef SYSTEM_THREADS64 SYSTEM_THREADS;
	typedef PSYSTEM_THREADS64 PSYSTEM_THREADS;

	typedef _SYSTEM_PROCESSES64 _SYSTEM_PROCESSES;
	typedef SYSTEM_PROCESSES64 SYSTEM_PROCESSES;
	typedef PSYSTEM_PROCESSES64 PSYSTEM_PROCESSES;
#endif // !defined(_WIN64) && !defined(WIN64)

	typedef enum _OBJECT_INFORMATION_CLASSEX {
		ObjectBasicInformation = 0,
		ObjectNameInformation,
		ObjectTypeInformation,
		ObjectAllInformation,
		ObjectDataInformation,
	} OBJECT_INFORMATION_CLASSEX;

	typedef DWORD(WINAPI *LPFN_ZwSuspendProcess)(HANDLE hProcess);
	typedef DWORD(WINAPI *LPFN_ZwResumeProcess)(HANDLE hProcess);
	typedef NTSTATUS(WINAPI *LPFN_ZwQueryInformationProcess)(HANDLE, PROCESSINFOCLASSEX, LPVOID, DWORD, PDWORD);
	typedef NTSTATUS(WINAPI *LPFN_ZwQuerySystemInformation)(DWORD, PVOID, DWORD, DWORD*);
	typedef NTSTATUS(WINAPI *LPFN_ZwQueryObject)(HANDLE, OBJECT_INFORMATION_CLASSEX, PVOID, ULONG, PULONG);
	typedef NTSTATUS(WINAPI *LPFN_RtlAdjustPrivilege)(DWORD, BOOL, BOOL, PDWORD);

	LPFN_ZwSuspendProcess ZwSuspendProcess;//������̵�API 
	LPFN_ZwResumeProcess ZwResumeProcess;//�ָ����̵�API
	LPFN_ZwQuerySystemInformation ZwQuerySystemInformation;

	void InitApis()
	{
		const _TCHAR * NTDLL_NAME = _T("NTDLL.DLL");
		ZwSuspendProcess = (LPFN_ZwSuspendProcess)GetProcAddress(GetModuleHandle(NTDLL_NAME), "ZwSuspendProcess");
		ZwResumeProcess = (LPFN_ZwResumeProcess)GetProcAddress(GetModuleHandle(NTDLL_NAME), "ZwResumeProcess");
		ZwQuerySystemInformation = (LPFN_ZwQuerySystemInformation)GetProcAddress(GetModuleHandle(NTDLL_NAME), "ZwQuerySystemInformation");
	}

	//��ȡ���̹���/����״̬
	BOOL GetProcessState(DWORD dwProcessId)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		ULONG cbSize = 0;
		BYTE *pvData = NULL;

		//ShowTips(_T("---------------------���н�����Ϣ----------------------------------------\n"));
		PSYSTEM_PROCESSES psp = NULL;
		status = (NTSTATUS)ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, NULL, 0, &cbSize);
		if (status == STATUS_INFO_LENGTH_MISMATCH) {
			pvData = (BYTE *)malloc(cbSize * sizeof(BYTE));
			status = (NTSTATUS)ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, (PVOID)pvData, cbSize, NULL);
			if (status == STATUS_SUCCESS)
			{
				psp = (PSYSTEM_PROCESSES)pvData;
				do {
					if (psp->ProcessId == dwProcessId)
					{
						if (psp->Threads[0].State == StateWait && psp->Threads[0].WaitReason == Suspended)
						{
							return TRUE;
						}
						else
						{
							return FALSE;
						}
					}
					psp = (PSYSTEM_PROCESSES)(SIZE_T)((SIZE_T)psp + psp->NextEntryDelta);
				} while (psp->NextEntryDelta != 0);

				free(pvData);
				pvData = NULL;
			}
			else if (status == STATUS_UNSUCCESSFUL) {
				//ShowMsgs(_T("��ʾ"), _T("\n STATUS_UNSUCCESSFUL"));
			}
			else if (status == STATUS_NOT_IMPLEMENTED) {
				//ShowMsgs(_T("��ʾ"), _T("\n STATUS_NOT_IMPLEMENTED"));
			}
			else if (status == STATUS_INVALID_INFO_CLASS) {
				//ShowMsgs(_T("��ʾ"), _T("\n STATUS_INVALID_INFO_CLASS"));
			}
			else if (status == STATUS_INFO_LENGTH_MISMATCH) {
				//ShowMsgs(_T("��ʾ"), _T("\n STATUS_INFO_LENGTH_MISMATCH"));
			}
		}

		return FALSE;
	}

	int GetNameFromPath(_TCHAR(&Name)[MAX_PATH], const _TCHAR * pPath)
	{
		int result = (-1);
		_TCHAR * pT = NULL;

		if (pPath && *pPath)
		{
			pT = _tcsrchr((_TCHAR *)pPath, _T('\\'));
			if (!pT)
			{
				pT = _tcsrchr((_TCHAR *)pPath, _T('/'));
			}
			if (pT && *pT)
			{
				_tcscpy(Name, pT + 1);
				result = 0;
			}
		}

		return result;
	}
	//�������̷���Ȩ��
	bool EnableDebugPriv()
	{
		HANDLE hToken;
		LUID sedebugnameValue;
		TOKEN_PRIVILEGES tkp;

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
			return false;
		}

		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) {
			CloseHandle(hToken);
			return false;
		}

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = sedebugnameValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {
			CloseHandle(hToken);
			return false;
		}

		return true;
	}
	//���̿��գ�ö�ٸ����̣�
	BOOL GetPidByProcessName(LPCTSTR lpszProcessName, DWORD & dwProcessId)
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
				dwProcessId = pe.th32ProcessID;
				return TRUE;
			}
		}

		::CloseHandle(hSnapshot);
		return FALSE;
	}

}

#include <map>

typedef std::map<std::string, void *> STRINGFUNPTRMAP;
typedef STRINGFUNPTRMAP::iterator STRINGFUNPTRMAPIT;
typedef STRINGFUNPTRMAP::value_type STRINGFUNPTRMAPPAIR;

typedef std::map<tstring, STRINGFUNPTRMAP> MODULESTRINGFUNPTRMAP;
typedef MODULESTRINGFUNPTRMAP::iterator MODULESTRINGFUNPTRMAPIT;
typedef MODULESTRINGFUNPTRMAP::value_type MODULESTRINGFUNPTRMAPPAIR;

//ע�붯̬�⵽Զ�̽���
BOOL InjectDllToRemoteProcess(const _TCHAR* lpDllName, DWORD dwProcessId);
//��Զ�̽���ж��DLL
BOOL UnInjectDllFromRemoteProcess(const _TCHAR* lpDllName, DWORD dwProcessId);

void Usage(const _TCHAR * pT)
{
	_TCHAR tFileName[MAX_PATH] = { 0 };
	UNK_API::GetNameFromPath(tFileName, pT);
	_tprintf(_T("Usage:\r\n")
		_T("%s -[i|I|u|U][n|N|s|S] [dll] [pid|procname] [workpath]\r\n")
		_T("\t===================================================================\r\n")
		_T("\t%s -in [dll] [pid] //install inject dll by pid\r\n")
		_T("\t%s -is [dll] [procname] [workpath] //install inject dll by procname\r\n")
		_T("\t%s -un [dll] [pid] //uninstall inject dll by pid\r\n")
		_T("\t%s -us [dll] [procname] //uninstall inject dll by procname\r\n")
		, tFileName, tFileName, tFileName, tFileName, tFileName);
}

int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bResult = FALSE;
	DWORD dwProcessId = 0;
	BOOL bSetupFlags = FALSE;
	_TCHAR tFileName[MAX_PATH] = { 0 };

	if (argc > 1 && _tcslen(argv[1]) > MAX_DEFAULTCHAR)
	{
		switch (argv[1][1])
		{
		case _T('i'):
		case _T('I'):
			bSetupFlags = TRUE;
			switch (argv[1][2])
			{
			case _T('n'):
			case _T('N'):
				{
					if (argc != 4)
					{
						Usage(argv[0]);
						return 0;
					}
					dwProcessId = _ttol(argv[3]);
				}
				break;		
			case _T('s'):
			case _T('S'):
				{
					if (argc != 5)
					{
						Usage(argv[0]);
						return 0;
					}
					PROCESS_INFORMATION pi = { 0 };
					STARTUPINFO si = { 0 };

					si.cb = sizeof(si);
					si.wShowWindow = SW_SHOW;
					si.dwFlags = STARTF_USESHOWWINDOW;

					bResult = CreateProcess(argv[3], NULL, NULL, FALSE, NULL, NULL/*CREATE_SUSPENDED*/, NULL, argv[4], &si, &pi);

					if (!bResult)
					{
						//��������ʧ��
						::MessageBox(NULL, _T("error "), _T("error"), MB_OK);
						return 0;
					}

					WaitForSingleObject(pi.hProcess, WAIT_TIMEOUT * 4);

					if (UNK_API::GetNameFromPath(tFileName, argv[3]) ||
						!UNK_API::GetPidByProcessName(tFileName, dwProcessId))
					{
						return 0;
					}
				}
				break;
			default:
				Usage(argv[0]);
				return 0;
				break;
			}
			break;

		case _T('u'):
		case _T('U'):
		{
			switch (argv[1][2])
			{
			case _T('n'):
			case _T('N'):
				{
					if (argc != 4)
					{
						Usage(argv[0]);
						return 0;
					}
					dwProcessId = _ttol(argv[3]);
				}
				break;
			case _T('s'):
			case _T('S'):
				{
					if (argc != 4)
					{
						Usage(argv[0]);
						return 0;
					}
					if (UNK_API::GetNameFromPath(tFileName, argv[3]) ||
						!UNK_API::GetPidByProcessName(tFileName, dwProcessId))
					{
						return 0;
					}
				}
				break;
			default:
				Usage(argv[0]);
				return 0;
				break;
			}
		}
			break;
		default:
			Usage(argv[0]);
			return 0;
			break;
		}
	}
	else
	{
		Usage(argv[0]);
		return 0;
	}

	UNK_API::EnableDebugPriv();
	
	UNK_API::InitApis();
	if (bSetupFlags)
	{
		bResult = InjectDllToRemoteProcess(argv[2], dwProcessId);
		if (!bResult)
		{
			//ע��Զ�̽���ʧ��
			::MessageBox(NULL, _T("Inject remote error!"), _T("Error"), MB_OK);
		}
	}
	else
	{
		bResult = UnInjectDllFromRemoteProcess(argv[2], dwProcessId);
		if (!bResult)
		{
			//��ע��Զ�̽���ʧ��
			::MessageBox(NULL, _T("Uninject remote error!"), _T("Error"), MB_OK);
		}
	}

    return 0;
}

/********************************************************************************************************/

//ע��DLL��Զ�̽���
BOOL InjectDllToRemoteProcess(const _TCHAR* lpDllName, DWORD dwProcessId)
{
	BOOL bResult = FALSE;
	SIZE_T stSize = 0;
	LPVOID lpRemoteBuffer = NULL;
	HANDLE hRemoteProcess = NULL;
	HANDLE hRemoteThread = NULL;
	DWORD  dwRemoteThreadId = 0;
	std::string strDllName = ("");
	SIZE_T stNumberOfBytesWritten = 0;
	LPTHREAD_START_ROUTINE pfnLoadLibraryA = NULL;
	LPTHREAD_START_ROUTINE pfnLoadLibraryW = NULL;
	LPTHREAD_START_ROUTINE pfnFreeLibrary = NULL;

	MODULESTRINGFUNPTRMAP msfmap = {
		{ _T("KERNEL32.DLL"),
			{
				{ ("LoadLibraryA"), NULL },
				{ ("LoadLibraryW"), NULL },
			} 
		}		
	};

	STRINGFUNPTRMAPIT itItemEnd;
	STRINGFUNPTRMAPIT itItemIdx;
	MODULESTRINGFUNPTRMAPIT itEnd = msfmap.end();
	MODULESTRINGFUNPTRMAPIT itIdx = msfmap.begin();
	for (; itIdx != itEnd; itIdx++)
	{
		itItemEnd = itIdx->second.end();
		itItemIdx = itIdx->second.begin();
		for (; itItemIdx != itItemEnd; itItemIdx++)
		{
			itItemIdx->second = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(itIdx->first.c_str()), itItemIdx->first.c_str());
		}
	}

    //����Pid�õ����̾��(ע�����Ȩ��)
	hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_SUSPEND_RESUME | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessId);
	if (hRemoteProcess == INVALID_HANDLE_VALUE)
    {
		goto __LEAVE_CLEAN__;
    }
	
	if (UNK_API::GetProcessState(dwProcessId))
	{
		//�������
		UNK_API::ZwSuspendProcess(hRemoteProcess);
	}

	strDllName = TToA(lpDllName);

    //����DLL·������Ҫ���ڴ�ռ�
	stSize = (strDllName.length() + 1);

    //ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ���������,�ɹ����ط����ڴ���׵�ַ.
	lpRemoteBuffer = VirtualAllocEx(hRemoteProcess, NULL, stSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!lpRemoteBuffer)
    {
		goto __LEAVE_CLEAN__;
    }

    //ʹ��WriteProcessMemory������DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ�,�ɹ�����TRUE.
	bResult = WriteProcessMemory(hRemoteProcess, lpRemoteBuffer, strDllName.c_str(), stSize, &stNumberOfBytesWritten);
	if (!bResult || stNumberOfBytesWritten != stSize)
    {
		goto __LEAVE_CLEAN__;
    }

    //����һ�����������̵�ַ�ռ������е��߳�(Ҳ��:����Զ���߳�),�ɹ��������߳̾��.
    //ע��:���̾������߱�PROCESS_CREATE_THREAD, PROCESS_QUERY_INFORMATION, PROCESS_VM_OPERATION, PROCESS_VM_WRITE,��PROCESS_VM_READ����Ȩ��
	hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)msfmap[_T("KERNEL32.DLL")]["LoadLibraryA"], lpRemoteBuffer, 0, &dwRemoteThreadId);
	if (hRemoteThread == INVALID_HANDLE_VALUE)
    {
		goto __LEAVE_CLEAN__;
    }

	//�ָ�Զ���߳�
	ResumeThread(hRemoteThread);
	//�ָ�����
	UNK_API::ZwResumeProcess(hRemoteProcess);

	//ע��ɹ��ͷž��
    WaitForSingleObject(hRemoteThread, INFINITE);
	CloseHandle(hRemoteThread);
	hRemoteThread = NULL;
	
	bResult = TRUE;

__LEAVE_CLEAN__:
	
	if (hRemoteThread)
	{
		CloseHandle(hRemoteThread);
		hRemoteThread = NULL;
	}

	if (hRemoteProcess)
	{
		if (lpRemoteBuffer && stSize)
		{
			VirtualFreeEx(hRemoteProcess, lpRemoteBuffer, stSize, MEM_DECOMMIT | MEM_RELEASE);
			lpRemoteBuffer = NULL;
		}
		CloseHandle(hRemoteProcess);
		hRemoteProcess = NULL;
	}
	
    return bResult;
}

//��Զ�̽���ж��DLL
BOOL UnInjectDllFromRemoteProcess(const _TCHAR* lpDllName, DWORD dwProcessId)
{
	BOOL bResult = FALSE;
	SIZE_T stSize = 0;
	DWORD dwModuleHandle = NULL;
	LPVOID lpRemoteBuffer = NULL;
	HANDLE hRemoteProcess = NULL;
	HANDLE hRemoteThread = NULL;
	DWORD  dwRemoteThreadId = 0;
	std::string strDllName = ("");
	SIZE_T stNumberOfBytesWritten = 0;
	LPTHREAD_START_ROUTINE pfnLoadLibraryA = NULL;
	LPTHREAD_START_ROUTINE pfnLoadLibraryW = NULL;
	LPTHREAD_START_ROUTINE pfnFreeLibrary = NULL;

	MODULESTRINGFUNPTRMAP msfmap = {
		{ _T("KERNEL32.DLL"),
			{
				{ ("FreeLibrary"), NULL },
				{ ("GetModuleHandleA"), NULL },
				{ ("GetModuleHandleW"), NULL },
			}
		}
	};

	STRINGFUNPTRMAPIT itItemEnd;
	STRINGFUNPTRMAPIT itItemIdx;
	MODULESTRINGFUNPTRMAPIT itEnd = msfmap.end();
	MODULESTRINGFUNPTRMAPIT itIdx = msfmap.begin();
	for (; itIdx != itEnd; itIdx++)
	{
		itItemEnd = itIdx->second.end();
		itItemIdx = itIdx->second.begin();
		for (; itItemIdx != itItemEnd; itItemIdx++)
		{
			itItemIdx->second = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(itIdx->first.c_str()), itItemIdx->first.c_str());
		}
	}

	//����Pid�õ����̾��(ע�����Ȩ��)
	hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_SUSPEND_RESUME | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessId);
	if (hRemoteProcess == INVALID_HANDLE_VALUE)
	{
		goto __LEAVE_CLEAN__;
	}

	if (UNK_API::GetProcessState(dwProcessId))
	{
		//�������
		UNK_API::ZwSuspendProcess(hRemoteProcess);
	}

	strDllName = TToA(lpDllName);

	//����DLL·������Ҫ���ڴ�ռ�
	stSize = (strDllName.length() + 1);

	//ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ���������,�ɹ����ط����ڴ���׵�ַ.
	lpRemoteBuffer = VirtualAllocEx(hRemoteProcess, NULL, stSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!lpRemoteBuffer)
	{
		goto __LEAVE_CLEAN__;
	}

	//ʹ��WriteProcessMemory������DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ�,�ɹ�����TRUE.
	bResult = WriteProcessMemory(hRemoteProcess, lpRemoteBuffer, strDllName.c_str(), stSize, &stNumberOfBytesWritten);
	if (!bResult || stNumberOfBytesWritten != stSize)
	{
		goto __LEAVE_CLEAN__;
	}

	if (UNK_API::GetProcessState(dwProcessId))
	{
		//�������
		UNK_API::ZwSuspendProcess(hRemoteProcess);
	}

	//ʹĿ����̵���GetModuleHandIe,���DLL�ڽ����еľ��  
	hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)msfmap[_T("KERNEL32.DLL")]["GetModuleHandleA"], lpRemoteBuffer, 0, &dwRemoteThreadId);
	
	//�ָ�Զ���߳�
	ResumeThread(hRemoteThread);
	//�ָ�����
	UNK_API::ZwResumeProcess(hRemoteProcess);

	//ע��ɹ��ͷž��
	WaitForSingleObject(hRemoteThread, INFINITE);
	//���GetModuleHandle�ķ���ֵ
	GetExitCodeThread(hRemoteThread, &dwModuleHandle);
	//�ͷ�Ŀ�����������Ŀռ�  
	VirtualFreeEx(hRemoteProcess, lpRemoteBuffer, stSize, MEM_RELEASE | MEM_DECOMMIT);
	
	CloseHandle(hRemoteThread);
	hRemoteThread = NULL;

	if (UNK_API::GetProcessState(dwProcessId))
	{
		//�������
		UNK_API::ZwSuspendProcess(hRemoteProcess);
	}
	//ʹĿ����̵���FreeLibrary,ж��DLL  
	hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)msfmap[_T("KERNEL32.DLL")]["FreeLibrary"], (LPVOID)dwModuleHandle, 0, &dwRemoteThreadId);
	
	//�ָ�Զ���߳�
	ResumeThread(hRemoteThread);
	//�ָ�����
	UNK_API::ZwResumeProcess(hRemoteProcess);

	//ע��ɹ��ͷž��
	WaitForSingleObject(hRemoteThread, INFINITE);
	//�ȴ�FreeLibraryж�����  
	WaitForSingleObject(hRemoteThread, INFINITE);
	CloseHandle(hRemoteThread);
	hRemoteThread = NULL;

	CloseHandle(hRemoteProcess);
	
	bResult = TRUE;

__LEAVE_CLEAN__:

	if (hRemoteThread)
	{
		CloseHandle(hRemoteThread);
		hRemoteThread = NULL;
	}

	if (hRemoteProcess)
	{
		if (lpRemoteBuffer && stSize)
		{
			VirtualFreeEx(hRemoteProcess, lpRemoteBuffer, stSize, MEM_DECOMMIT | MEM_RELEASE);
			lpRemoteBuffer = NULL;
		}
		CloseHandle(hRemoteProcess);
		hRemoteProcess = NULL;
	}

	exit(0);

	return bResult;
}
