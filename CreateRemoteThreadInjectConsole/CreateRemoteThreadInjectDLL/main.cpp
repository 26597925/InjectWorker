#include <tchar.h>
#include "main.h"

//ԭ�������Ͷ���
typedef int (WINAPI* MsgBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
typedef HWND(WINAPI* PCreateMutexW)(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner, LPCTSTR lpName);

MsgBoxW OldMsgBoxW=NULL;//ָ��ԭ������ָ��
FARPROC pfOldMsgBoxW;  //ָ������Զָ��

PCreateMutexW OldCreateMutexW=NULL;//ָ��ԭ������ָ��
FARPROC pfOldCreateMutexW;  //ָ������Զָ��

BYTE OldCode[5]; //ԭϵͳAPI��ڴ���
BYTE NewCode[5]; //ԭϵͳAPI�µ���ڴ���(jmp xxxxxxxx)

HANDLE hProcess = NULL;//��������̾��
HINSTANCE hInst = NULL;//API���ڵ�dll�ļ����

void HookOn();
void HookOff();
int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    HookOff();//����ԭ����֮ǰ���ǵ��Ȼָ�HOOKѽ����Ȼ�ǵ��ò�����
    //������ָ�HOOK���͵���ԭ�������������ѭ��
    //�Ͼ����õĻ������ǵĺ������Ӷ���ɶ�ջ��������������

    int nRet = ::MessageBoxW(hWnd, L"MessageBoxW is HOOK", lpCaption, uType);

    HookOn();//������ԭ�����󣬼ǵü�������HOOK����Ȼ�´λ�HOOK������

    return nRet;
}
HWND WINAPI MyCreateMutexW(LPCTSTR lpName, LPCTSTR lpCName)
{
	HWND hHandle = 0;
    //TRACE(lpText);
	HookOff();//����ԭ����֮ǰ���ǵ��Ȼָ�HOOKѽ����Ȼ�ǵ��ò�����
			  //������ָ�HOOK���͵���ԭ�������������ѭ��
			  //�Ͼ����õĻ������ǵĺ������Ӷ���ɶ�ջ��������������
    ::MessageBoxW(NULL,L"haha,CreateMutexW is HOOK",L"Message",MB_OK);
	hHandle=::FindWindowW(NULL, L"WECHAT.EXE_NEW");

	HookOn();//������ԭ�����󣬼ǵü�������HOOK����Ȼ�´λ�HOOK������

	return hHandle;
}


//�������ӵĺ���
void HookOn()
{
     if ( NULL == hProcess)
    {
        return;
    }

    DWORD dwTemp=0;
    DWORD dwOldProtect;
    //�޸�API�������ǰ5���ֽ�Ϊjmp xxxxxx
    //VirtualProtectEx(hProcess,(PVOID)pfOldMsgBoxW,5,PAGE_READWRITE,&dwOldProtect);
    //WriteProcessMemory(hProcess,(LPVOID)pfOldMsgBoxW,NewCode,5,0);
    //VirtualProtectEx(hProcess,(PVOID)pfOldMsgBoxW,5,dwOldProtect,&dwTemp);

    //�޸�API�������ǰ5���ֽ�Ϊjmp xxxxxx
    VirtualProtectEx(hProcess,(PVOID)pfOldCreateMutexW,5,PAGE_READWRITE,&dwOldProtect);
    WriteProcessMemory(hProcess,(LPVOID)pfOldCreateMutexW,NewCode,5,0);
    VirtualProtectEx(hProcess,(PVOID)pfOldCreateMutexW,5,dwOldProtect,&dwTemp);
}

//�رչ��ӵĺ���
void HookOff()
{
    if ( NULL == hProcess)
    {
        return;
    }

    DWORD dwTemp=0;
    DWORD dwOldProtect;

     //�ָ�API�������ǰ5���ֽ�
 //VirtualProtectEx(hProcess,(PVOID)pfOldMsgBoxW,5,PAGE_READWRITE,&dwOldProtect);
 //WriteProcessMemory(hProcess,(LPVOID)pfOldMsgBoxW,OldCode,5,0);
 //VirtualProtectEx(hProcess,(PVOID)pfOldMsgBoxW,5,dwOldProtect,&dwTemp);

  //�ָ�API�������ǰ5���ֽ�
 VirtualProtectEx(hProcess,(PVOID)pfOldCreateMutexW,5,PAGE_READWRITE,&dwOldProtect);
 WriteProcessMemory(hProcess,(LPVOID)pfOldCreateMutexW,OldCode,5,0);
 VirtualProtectEx(hProcess,(PVOID)pfOldCreateMutexW,5,dwOldProtect,&dwTemp);
}

//��ȡAPI�������ǰ���ֽ�
//�����ǰ���ֽڱ�����ǰ�涨����ֽ�����BYTE OldCode[5]
//�����ǰ���ֽڱ�����ǰ�涨����ֽ�����BYTE NewCode[5]
void GetApiEntrance()
{
    //��ȡԭAPI��ڵ�ַ
    HMODULE hmod = ::LoadLibrary( _T("User32.dll" ));
    OldMsgBoxW = (MsgBoxW)::GetProcAddress(hmod, "MessageBoxW");
    pfOldMsgBoxW = (FARPROC)OldMsgBoxW;

    if (NULL == pfOldMsgBoxW)
    {
        ::MessageBox(NULL, _T("get APIentry error"), _T("error!"), 0);
        return;
    }
    hmod = ::LoadLibrary(_T("Kernel32.dll"));
    OldCreateMutexW=(PCreateMutexW)::GetProcAddress(hmod,"CreateMutexW");
    pfOldCreateMutexW=(FARPROC)OldCreateMutexW;

    if (pfOldCreateMutexW==NULL)
    {
        MessageBox(NULL,_T("��ȡԭAPI��ڵ�ַ����"),_T("error!"),0);
        return;
    }
  // ��ԭAPI�����ǰ5���ֽڴ��뱣�浽OldCode[]
  // ��ԭAPI�����ǰ5���ֽڴ��뱣�浽OldCode[]
	_asm
	{
		lea edi, OldCode		//��ȡOldCode����ĵ�ַ,�ŵ�edi
		mov esi, pfOldCreateMutexW //��ȡԭAPI��ڵ�ַ���ŵ�esi
		cld	  //�����־λ��Ϊ��������ָ����׼��
		movsd //����ԭAPI���ǰ4���ֽڵ�OldCode����
		movsb //����ԭAPI��ڵ�5���ֽڵ�OldCode����
	}

    NewCode[0]=0xe9;//ʵ����xe9���൱��jmpָ��

    //��ȡMyMessageBoxW����Ե�ַ,ΪJmp��׼��
    //int nAddr= UserFunAddr ?SysFunAddr - �����Ƕ��Ƶ�����ָ��Ĵ�С��;
    //Jmp nAddr;
    //�����Ƕ��Ƶ�����ָ��Ĵ�С��, �����ǣ����ֽ���
	_asm
	{
		lea eax, MyCreateMutexW //��ȡ���ǵ�MyMessageBoxW������ַ
		mov ebx, pfOldCreateMutexW  //ԭϵͳAPI������ַ
		sub eax, ebx			 //int nAddr= UserFunAddr �C SysFunAddr
		sub eax, 5			 //nAddr=nAddr-5
		mov dword ptr[NewCode + 1], eax //������ĵ�ַnAddr���浽NewCode����4���ֽ�
										//ע��һ��������ַռ4���ֽ�
	}

    //�����ϣ�����NewCode[]���ָ���൱��Jmp MyMessageBoxW
    //��Ȼ�Ѿ���ȡ����Jmp MyMessageBoxW
    //���ڸ��ǽ�Jmp MyMessageBoxWд��ԭAPI���ǰ���ֽڵ�ʱ����
    //֪��Ϊʲô�Ǹ��ֽ���
    //Jmpָ���൱��xe9,ռһ���ֽڵ��ڴ�ռ�
    //MyMessageBoxW��һ����ַ����ʵ��һ��������ռ���ֽڵ��ڴ�ռ�
    //int n=0x123;   nռ���ֽں�MyMessageBoxWռ���ֽ���һ����
    //1+4=5��֪��Ϊʲô�Ǹ��ֽ��˰�
    HookOn();
}

// a sample exported function
void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            {
				MessageBox(NULL, _T("ע��ɹ�"), _T("��ʾ"), MB_ICONINFORMATION);
                DWORD dwPid=::GetCurrentProcessId();
                hProcess=OpenProcess(PROCESS_ALL_ACCESS,0,dwPid);
                GetApiEntrance();
            }
            break;

		case DLL_PROCESS_DETACH:
			// detach from process
			{
				MessageBox(NULL, _T("��ע��ɹ�"), _T("��ʾ"), MB_ICONINFORMATION);
				HookOff();
			}
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
