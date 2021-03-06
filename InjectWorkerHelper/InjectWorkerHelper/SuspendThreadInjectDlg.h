// SuspendThreadInjectDlg.h : interface of the CSuspendThreadInjectDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CodeDataBase.h"
#include <DialogLayout.h>

class CSuspendThreadInjectDlg :
	public CCodeDataBase,
	public CDialogImpl<CSuspendThreadInjectDlg>,
	public CDialogLayout<CSuspendThreadInjectDlg>,
	public CUpdateUI<CSuspendThreadInjectDlg>,
	public CMessageFilter,
	public CIdleHandler
{
public:
	enum { IDD = IDD_DIALOG_SUSPENDTHREADINJECT };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CSuspendThreadInjectDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CSuspendThreadInjectDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_NOTIFY(OnNotify)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_SELECTAPPDLL, OnSelectAppDll)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_SUSPENDTHREADINJECT, OnSuspendThreadInject)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_RELEASEMEMORYSPACES, OnReleaseMemorySpaces)
		COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		CHAIN_MSG_MAP(CDialogLayout<CSuspendThreadInjectDlg>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_LAYOUT_MAP()
		//BEGIN_LAYOUT_CONTAINER(ABS(0.0), ABS(0.0), RATIO(1.0), RATIO(0.5))
		//LAYOUT_CONTROL(IDC_LIST_PROCESSES, LAYOUT_ANCHOR_ALL)
		//END_LAYOUT_CONTAINER()
		//BEGIN_LAYOUT_CONTAINER(ABS(0.0), RATIO(0.5), RATIO(1.0), RATIO(1.0))
		//LAYOUT_CONTROL(IDC_LIST_MODULES, LAYOUT_ANCHOR_ALL)
		//END_LAYOUT_CONTAINER()
		//LAYOUT_CONTROL(IDOK, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
		//LAYOUT_CONTROL(IDCANCEL, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
		//LAYOUT_CONTROL(ID_APP_ABOUT, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
	END_LAYOUT_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(HWND, LPARAM)
	{
		m_hProcess = NULL;
		m_lpBuffer = NULL;

		// center the dialog on the screen
		CenterWindow();

		m_SystemMenu = GetSystemMenu(FALSE);
		if (m_SystemMenu.m_hMenu != NULL)
		{
			m_SystemMenu.AppendMenu(MF_SEPARATOR);
			m_SystemMenu.AppendMenu(MF_BYCOMMAND, ID_APP_ABOUT, _T("关于"));
		}

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

		m_edtAppPid = GetDlgItem(IDC_EDIT_APPPID);
		m_edtAppDll = GetDlgItem(IDC_EDIT_APPDLL);
		m_btnSelectAppDll = GetDlgItem(IDC_BUTTON_SELECTAPPDLL);
		m_btnSuspendThreadInject = GetDlgItem(IDC_BUTTON_SUSPENDTHREADINJECT);
		m_btnReleaseMemorySpaces = GetDlgItem(IDC_BUTTON_RELEASEMEMORYSPACES);

		{
			this->SetWindowText(_T("挂起线程注入"));
			m_btnSelectAppDll.SetWindowText(_T("选择文件"));
			m_btnSuspendThreadInject.SetWindowText(_T("挂起注入"));
			m_btnReleaseMemorySpaces.SetWindowText(_T("释放内存"));
		}

		m_bGripper = TRUE;

		SetMsgHandled(FALSE);

		return TRUE;
	}

	LRESULT OnNotify(INT nID, LPNMHDR lpNMHDR)
	{
		switch (lpNMHDR->code)
		{
		case NM_RCLICK:
		{
			//if (m_ListViewCtrlData.GetNextItem(-1, LVNI_SELECTED) != -1)
			{
				CMenu menu;
				POINT point = { 0 };
				GetCursorPos(&point);
				//动态创建弹出式菜单对象
				if (menu.CreatePopupMenu())
				{
					//menu.AppendMenu(MF_STRING, IDM_SUBMENU_OPEN, _T("打开进程"));
					//menu.AppendMenu(MF_STRING, IDM_SUBMENU_ENUMOBJS, _T("枚举对象"));
					//menu.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_VERPOSANIMATION | TPM_LEFTALIGN | TPM_VERTICAL, point.x, point.y, m_hWnd);
					//menu.DestroyMenu();
				}
			}
		}
		break;
		//case LVN_COLUMNCLICK:
		case HDN_ITEMCLICK:
		{
			LPNMHEADER lpNMHEADER = (LPNMHEADER)lpNMHDR;
			if (this->NColumnSortItem() != lpNMHEADER->iItem)
			{
				this->NColumnSortItem(lpNMHEADER->iItem);
				this->BColumnSortFlag(true);
			}
			else
			{
				this->BColumnSortFlag(!this->BColumnSortFlag());
			}

			int nStartPos = 0;
			int nFinalPos = 0;
			_TCHAR tValue[MAXBYTE] = { 0 };
			CString strText = this->STRColumnProperty();
			for (int i = 0;
				i < lpNMHEADER->iItem
				&& (nStartPos = strText.Find(_T("|"), nStartPos + 1));
			i++){
				;
			}
			nFinalPos = strText.Find(_T("|"), nStartPos + 1);
			lstrcpyn(tValue, (LPCTSTR)strText + nStartPos + 1, nFinalPos - nStartPos);
			this->NColumnDataType((COLUMN_DATATYPE)_ttol(tValue));
			//this->LISTVIEWCTRLWINDOW(m_ListViewCtrlData);
			//m_ListViewCtrlData.SortItems(&CompareProcess, (LPARAM)this);
		}
		break;
		case LVN_ITEMCHANGED:
		{
			//if (m_ListViewCtrlData.GetNextItem(-1, LVNI_SELECTED) != -1)
			{
				DWORD dwProcessID = 0;
				_TCHAR tText[MAXBYTE] = { 0 };
				LVITEM lvi = { 0 };
				lvi.pszText = tText;
				lvi.cchTextMax = sizeof(tText) / sizeof(_TCHAR);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				//if (m_ListViewCtrlData.GetSelectedItem(&lvi))
				{
					dwProcessID = _ttol(tText);
					if (dwProcessID > 0)
					{
						//OnShowmoudles(dwProcessID);
					}
				}
			}
		}
		break;
		default:
			break;
		}

		return 0;
	}

	void OnDestroy()
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);
	}
	void About()
	{
		CAboutDlg dlg;
		dlg.DoModal();
	}
	void OnSysCommand(UINT nID, CPoint point)
	{
		switch (nID)
		{
		case ID_APP_ABOUT:
		{
			About();
			SetMsgHandled(TRUE);
		}
		break;
		default:
		{
			SetMsgHandled(FALSE);
		}
		break;
		}
	}
	
	LRESULT OnSelectAppDll(UINT, int, HWND)
	{
		BOOL bResult = FALSE;
		_TCHAR tFileName[MAX_PATH] = { 0 };

		if (SelectOpenFile(tFileName, _T("All Files(*.*)\0*.*\0Dll Files(*.DLL)\0*.DLL\0\0")))
		{
			m_edtAppDll.SetWindowText(tFileName);
		}

		return 0;
	}

	LRESULT OnSuspendThreadInject(UINT, int, HWND)
	{
		// TODO:  在此添加控件通知处理程序代码
		//打开目标进程
		BOOL   bRet = 0;

		THREAD_INJECT_CODE ic = { 0 };
		THREADENTRY32 te32 = { 0 };
		HANDLE hThreadSnap = NULL;
		DWORD dwThreadId = 0;
		HANDLE hThread = NULL;
		CONTEXT oldContext = { 0 };
		CONTEXT newContext = { 0 };
		DWORD dwOldEip = 0;

		DWORD dwProcessID = 0;
		CString strAppPid(_T(""));
		CString strAppDll(_T(""));

		m_edtAppPid.GetWindowText(strAppPid);
		m_edtAppDll.GetWindowText(strAppDll);

		dwProcessID = _ttol(strAppPid);

		//打开进程
		m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);

		if (!m_hProcess)
		{
			MessageBox(_T("OpenProcess 失败"));
			return 0;
		}


		m_lpBuffer = VirtualAllocEx(m_hProcess, NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!m_lpBuffer)
		{
			MessageBox(_T("VirtualAllocEx 失败"));
			return 0;
		}

		//给ShellCode结构体赋值
		ic.byPUSH = 0x68;
		ic.dwPUSH_VALUE = 0x12345678;
		ic.byPUSHFD = 0x9C;
		ic.byPUSHAD = 0x60;
		ic.byMOV_EAX = 0xB8;
		ic.dwMOV_EAX_VALUE = (DWORD)m_lpBuffer + offsetof(THREAD_INJECT_CODE, szDllPath);
		ic.byPUSH_EAX = 0x50;
		ic.byMOV_ECX = 0xB9;
		ic.dwMOV_ECX_VALUE = (DWORD)&LoadLibrary;
		ic.wCALL_ECX = 0xD1FF;
		ic.byPOPAD = 0x61;
		ic.byPOPFD = 0x9D;
		ic.byRETN = 0xC3;
		memcpy(ic.szDllPath, CStringA(strAppDll), CStringA(strAppDll).GetLength());

		//写入ShellCode
		bRet = WriteProcessMemory(m_hProcess, m_lpBuffer, &ic, sizeof(ic), NULL);
		if (!bRet)
		{
			MessageBox(_T("写入内存失败"));
			return 0;
		}

		//创建线程快照查找目标程序主线程
		te32.dwSize = sizeof(te32);
		hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (hThreadSnap == INVALID_HANDLE_VALUE)
		{
			MessageBox(_T("CreateToolhelp32Snapshot 失败"));
			return 0;
		}

		//遍历查询目标程序主线程ID
		if (Thread32First(hThreadSnap, &te32))
		{
			do
			{
				if (dwProcessID == te32.th32OwnerProcessID)
				{
					dwThreadId = te32.th32ThreadID;
					break;
				}
			} while (Thread32Next(hThreadSnap, &te32));
		}

		//打开目标主线程
		hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);

		if (!OpenThread)
		{
			MessageBox(_T("OpenThread 失败"));
			return 0;
		}

		//挂起目标主线程
		bRet = SuspendThread(hThread);

		if (bRet == -1)
		{
			MessageBox(_T("SuspendThread 失败"));
			return 0;
		}

		oldContext.ContextFlags = CONTEXT_FULL;
		bRet = GetThreadContext(hThread, &oldContext);
		if (!bRet)
		{
			MessageBox(_T("GetThreadContext 失败"));
			return 0;
		}
		newContext = oldContext;


#ifdef _WIN64
		newContext.Rip = (DWORD)m_lpBuffer;
		dwOldEip = newContext.Rip;
#else 
		newContext.Eip = (DWORD)m_lpBuffer;
		dwOldEip = newContext.Eip;
#endif

		//;将指针指向ShellCode第一句push 12345678h中的地址,写入返回地址
		bRet = WriteProcessMemory(m_hProcess, ((char*)m_lpBuffer) + 1, &dwOldEip, sizeof(DWORD), NULL);
		if (!bRet)
		{
			MessageBox(_T("写入内存失败"));
			return 0;
		}

		bRet = SetThreadContext(hThread, &newContext);

		if (!bRet)
		{
			MessageBox(_T("SetThreadContext 失败"));
			return 0;
		}

		//然后把主线程跑起来
		bRet = ResumeThread(hThread);

		if (bRet == -1)
		{
			MessageBox(_T("ResumeThread 失败"));
			return 0;
		}
		return 0;
	}

	LRESULT OnReleaseMemorySpaces(UINT, int, HWND)
	{
		if (m_hProcess && m_lpBuffer)
		{
			if (!VirtualFreeEx(m_hProcess, m_lpBuffer, 0, MEM_DECOMMIT | MEM_RELEASE))
			{
				MessageBox(_T("VirtualFreeEx 失败"));
				return 0;
			}
			else
			{
				MessageBox(_T("释放对方空间成功"));
			}			
		}

		return 0;
	}

	LRESULT OnAppAbout(UINT, int, HWND)
	{
		About();
		return 0;
	}

	LRESULT OnOK(UINT, int nID, HWND)
	{
		// TODO: Add validation code 
		CloseDialog(nID);
		return 0;
	}

	LRESULT OnCancel(UINT, int nID, HWND)
	{
		CloseDialog(nID);
		return 0;
	}

	void CloseDialog(int nValue)
	{
		EndDialog(nValue);
		//DestroyWindow();
		//::PostQuitMessage(nValue);
	}

private:
	HANDLE m_hProcess;
	LPVOID m_lpBuffer;

	CMenuT<false> m_SystemMenu;
	CEditT<CWindow> m_edtAppPid;
	CEditT<CWindow> m_edtAppDll;
	CButtonT<CWindow> m_btnSelectAppDll;
	CButtonT<CWindow> m_btnSuspendThreadInject;
	CButtonT<CWindow> m_btnReleaseMemorySpaces;
};
