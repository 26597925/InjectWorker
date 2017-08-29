// IMEInjectDlg.h : interface of the CIMEInjectDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CodeDataBase.h"
#include <DialogLayout.h>
#include <Imm.h>
#pragma comment(lib,"IMM32.lib")

class CIMEInjectDlg :
	public CCodeDataBase,
	public CDialogImpl<CIMEInjectDlg>,
	public CDialogLayout<CIMEInjectDlg>,
	public CUpdateUI<CIMEInjectDlg>,
	public CMessageFilter,
	public CIdleHandler
{
public:
	enum { IDD = IDD_DIALOG_IMEINJECT };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CIMEInjectDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CIMEInjectDlg)
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
		CHAIN_MSG_MAP(CDialogLayout<CIMEInjectDlg>)
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
		// center the dialog on the screen
		CenterWindow();

		m_SystemMenu = GetSystemMenu(FALSE);
		if (m_SystemMenu.m_hMenu != NULL)
		{
			m_SystemMenu.AppendMenu(MF_SEPARATOR);
			m_SystemMenu.AppendMenu(MF_BYCOMMAND, ID_APP_ABOUT, _T("����"));
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
			this->SetWindowText(_T("���뷨ע��"));
			m_btnSelectAppDll.SetWindowText(_T("ѡ���ļ�"));
			m_btnSuspendThreadInject.SetWindowText(_T("����ע��"));
			m_btnReleaseMemorySpaces.SetWindowText(_T("�ָ�ע��"));
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
				//��̬��������ʽ�˵�����
				if (menu.CreatePopupMenu())
				{
					//menu.AppendMenu(MF_STRING, IDM_SUBMENU_OPEN, _T("�򿪽���"));
					//menu.AppendMenu(MF_STRING, IDM_SUBMENU_ENUMOBJS, _T("ö�ٶ���"));
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
		// TODO:  �ڴ���ӿؼ�֪ͨ����������
		_TCHAR ImeDllPath[MAX_PATH] = { 0 };
		_TCHAR InjectDllPath[MAX_PATH] = { 0 };

#ifdef _WIN64
		_sntprintf(ImeDllPath, sizeof(ImeDllPath) / sizeof(_TCHAR), _T("%sInjectImeDllX64.dll"), GetProgramPath().c_str());
		_sntprintf(InjectDllPath, sizeof(InjectDllPath) / sizeof(_TCHAR), _T("%sInjectHelperX64.dll"), GetProgramPath().c_str());
#else
		_sntprintf(ImeDllPath, sizeof(ImeDllPath) / sizeof(_TCHAR), _T("%sInjectImeDll.dll"), GetProgramPath().c_str());
		_sntprintf(InjectDllPath, sizeof(InjectDllPath) / sizeof(_TCHAR), _T("%sInjectHelper.dll"), GetProgramPath().c_str());
#endif
		
		//ԭʼ���뷨
		HKL HklOldInput = NULL;
		//�µ����뷨
		HKL HklNewInput = NULL;
		//���뷨��������
		TCHAR ImeSymbol[MAX_PATH];
		//��ע����̵Ĵ��ھ��
		HWND hWnd = NULL;
		HWND hTopWnd = NULL;

		HKEY phkResult;
		int i = 0;
		TCHAR ValueName[MAX_PATH];  //�洢�õ��ļ�ֵ��
		TCHAR lpData[MAX_PATH];     //�洢�õ��ļ�ֵ����
		DWORD lenValue = MAX_PATH;  //�洢��ֵ�����ݳ���
		DWORD lenData = MAX_PATH;   //�洢��ֵ�����ݳ���

		DWORD dwProcessID = 0;
		CString strAppPid(_T(""));
		CString strAppDll(_T(""));

		m_edtAppPid.GetWindowText(strAppPid);
		m_edtAppDll.GetWindowText(strAppDll);

		dwProcessID = _ttol(strAppPid);

		//1.����ԭʼ�ļ��̲��֣�������滹ԭ
		SystemParametersInfo(SPI_GETDEFAULTINPUTLANG, 0, &HklOldInput, 0);

		//2.�����뷨DLL�ʹ�ע��Dllһ��ŵ�C:\\WINDOWS\\SYSTEM32Ŀ¼��
		CopyFile(ImeDllPath, tstring(GetSystemPath() + _T("InjectImeDll.ime")).c_str(), FALSE);
		CopyFile(strAppDll, tstring(GetSystemPath() + _T("InjectHelper.dll")).c_str(), FALSE);

		//3.�������뷨
		HklNewInput = ImmInstallIME(tstring(GetSystemPath() + _T("InjectImeDll.ime")).c_str(), _T("�ҵ����뷨"));

		//4.�ж����뷨����Ƿ���Ч
		if (!ImmIsIME(HklNewInput))
		{
			MessageBox(_T("���뷨����ʧ��!!!"));
		}

		//5.���������뷨�ļ��̲���
		ActivateKeyboardLayout(HklNewInput, 0);
		//6.��ȡ����������뷨ע�����������ɾ��
		GetKeyboardLayoutName(ImeSymbol);

		//7.��ȡ���ھ��
		hWnd = GetHwndByProcessId(dwProcessID);

		//8.����ָ�����ڵ����뷨
		if (HklNewInput != NULL)
		{
			::PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0x1, (LPARAM)HklNewInput);
		}

		Sleep(WAIT_TIMEOUT * 4);

		//9.��ԭ���̲���
		SystemParametersInfo(SPI_SETDEFAULTINPUTLANG, 0, &HklOldInput, SPIF_SENDWININICHANGE);

		//10.���㴰�ڻ���ȥ��������ȥ����ж�ز�������Ϊime�ļ�ռ��
		do
		{
			hTopWnd = ::FindWindowEx(NULL, hTopWnd, NULL, NULL);
			if (hTopWnd != NULL)
			{
				::PostMessage(hTopWnd, WM_INPUTLANGCHANGEREQUEST, 0x1, (LPARAM)&HklOldInput);
			}
		} while (hTopWnd == NULL);

		//11.ж�����뷨
		if (HklNewInput != NULL)
		{
			UnloadKeyboardLayout(HklNewInput);
		}

		//12.ɾ��ע��������ۼ�
		//��ע�����Ŀ����ȡ���
		RegOpenKey(HKEY_CURRENT_USER, _T("Keyboard Layout\\Preload"), &phkResult);
		//ö��ע�����Ŀ
		while (RegEnumValue(phkResult, i, ValueName, &lenValue, NULL, NULL, (LPBYTE)lpData, &lenData) != ERROR_NO_MORE_ITEMS)
		{
			if (lenData != 0)
			{
				if (_tcscmp(ImeSymbol, lpData) == 0)
				{
					//ɾ����Ŀ��ֵ
					RegDeleteValue(phkResult, ValueName);
					break;
				}
			}

			memset(lpData, 0, sizeof(lpData));
			memset(ValueName, 0, sizeof(ValueName));
			lenValue = MAX_PATH;
			lenData = MAX_PATH;
			i++;
		}

		// ɾ�����뷨�ļ�
		DeleteFile(tstring(GetSystemPath() + _T("InjectImeDll.ime")).c_str());

		return 0;
	}

	LRESULT OnReleaseMemorySpaces(UINT, int, HWND)
	{
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

	CMenuT<false> m_SystemMenu;
	CEditT<CWindow> m_edtAppPid;
	CEditT<CWindow> m_edtAppDll;
	CButtonT<CWindow> m_btnSelectAppDll;
	CButtonT<CWindow> m_btnSuspendThreadInject;
	CButtonT<CWindow> m_btnReleaseMemorySpaces;
};
