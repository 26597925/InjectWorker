// HijackInjectDlg.h : interface of the CHijackInjectDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CodeDataBase.h"
#include <DialogLayout.h>

CHAR *g_pszHeader = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// ͷ�ļ�\r\n"
"#include <windows.h>\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n";


CHAR *g_pszHeader2 = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// �궨��\r\n"
"#define NAKED __declspec(naked)\r\n"
"#define EXPORT __declspec(dllexport)\r\n"
"#define MYEXPORTFUN EXPORT NAKED\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"//ȫ�ֱ���\r\n"
"HMODULE hDll = NULL;\r\n"
"DWORD dwRetaddress[%d];							//��ŷ��ص�ַ\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// �ڲ����� ��ȡ��ʵ������ַ\r\n"
"FARPROC WINAPI GetAddress(PCSTR pszProcName)\r\n"
"{\r\n"
"\t\tFARPROC fpAddress;\r\n"
"\t\tCHAR szTemp[MAX_PATH] = { 0 };\r\n"
"\t\tfpAddress = GetProcAddress(hDll, pszProcName);\r\n"
"\t\tif (fpAddress == NULL)\r\n"
"\t\t{\r\n"
"\t\t\t\twsprintf(szTemp,\"�޷��ҵ����� :%s �ĵ�ַ \", pszProcName);\r\n"
"\t\t\t\tMessageBoxA(NULL, szTemp, \"����\", MB_OK);\r\n"
"\t\t\t\tExitProcess(-2);\r\n"
"\t\t}\r\n"
"\t\t//������ʵ��ַ\r\n"
"\t\treturn fpAddress;\r\n"
"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"// DLL MAIN\r\n"
"BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)\r\n"
"{\r\n"
"\t\tswitch (fdwReason)\r\n"
"\t\t{\r\n"
"\t\tcase DLL_PROCESS_ATTACH:\r\n"
"\t\t\t\thDll = LoadLibrary(\"�޸�ΪҪ�ͽٳֵ�DLL\");\r\n"
"\t\t\t\tif (!hDll)\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\tMessageBox(\"��Ҫ�ٳֵ�DLL�����ڣ�ע�����\");\r\n"
"\t\t\t\t\treturn FALSE;\r\n"
"\t\t\t\t}\r\n"
"\t\t\t\tMessageBox(\"���ӳɹ�\");\r\n"
"\t\t\tbreak;\r\n"
"\t\tcase DLL_PROCESS_DETACH:\r\n"
"\t\t\t\tif (hDll != NULL)\r\n"
"\t\t\t\t\tFreeLibrary(hDll); \r\n"
"\t\t\tbreak; \r\n"
"\t\t}\r\n"
"\t\treturn TRUE;\r\n"

"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n";


CHAR *g_pszFun = "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n"
"//�������� %d  ��Ҫ����ı����붨��Ϊȫ�֣�������ж�ջƽ������\r\n"
"MYEXPORTFUN My_%s()\r\n"
"{\r\n"
"\t\tGetAddress(\"%s\");\r\n"
"\t\t//��ʱջ�����ֵ��Ƿ��ص�ַ\r\n"
"\t\t__asm pop dwRetaddress[%d]				//����ԭ������ַ\r\n"
"\t\t//����ԭ����\r\n"
"\t\t__asm call eax\r\n"
"\t\t__asm jmp dword ptr dwRetaddress[%d]			//����ԭ����\r\n"
"}\r\n"
"////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n\r\n";


class CHijackInjectDlg :
	public CCodeDataBase,
	public CDialogImpl<CHijackInjectDlg>,
	public CDialogLayout<CHijackInjectDlg>,
	public CUpdateUI<CHijackInjectDlg>,
	public CMessageFilter,
	public CIdleHandler
{
public:
	enum { IDD = IDD_DIALOG_HIJACKINJECT };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CHijackInjectDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CHijackInjectDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_NOTIFY(OnNotify)
		MSG_WM_DROPFILES(OnDropFiles)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_SELECTAPPEXE, OnSelectAppExe)
		COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		CHAIN_MSG_MAP(CDialogLayout<CHijackInjectDlg>)
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
		m_bIsFirst = TRUE;

		m_strCodeFileName = _T("D:\\MyCodeDll.c");

		memset(&m_MapFileStruct, 0, sizeof(m_MapFileStruct));

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

		RegisterDropFilesEvent(this->m_hWnd);

		m_btnSelectAppExe = GetDlgItem(IDC_BUTTON_SELECTAPPEXE);
		m_ListViewCtrlData = GetDlgItem(IDC_LIST_DATA);

		{
			this->SetWindowText(_T("��̬��ٳ�"));
			m_btnSelectAppExe.SetWindowText(_T("���ɽٳִ���"));

			::SetWindowLong(m_ListViewCtrlData.m_hWnd, GWL_STYLE, ::GetWindowLong(m_ListViewCtrlData.m_hWnd, GWL_STYLE) | LVS_SINGLESEL);
			::SetWindowLong(m_ListViewCtrlData.m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_ListViewCtrlData.m_hWnd, GWL_EXSTYLE) | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
			m_ListViewCtrlData.SetExtendedListViewStyle(m_ListViewCtrlData.GetExtendedListViewStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
			m_ListViewCtrlData.InsertColumn(0, _T("Ordinal"), LVCFMT_LEFT, 100);
			m_ListViewCtrlData.InsertColumn(1, _T("Rva"), LVCFMT_LEFT, 100);
			m_ListViewCtrlData.InsertColumn(2, _T("Function Name"), LVCFMT_LEFT, 100);

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
			if (m_ListViewCtrlData.GetNextItem(-1, LVNI_SELECTED) != -1)
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
			this->LISTVIEWCTRLWINDOW(m_ListViewCtrlData);
			m_ListViewCtrlData.SortItems(&CompareProcess, (LPARAM)this);
		}
		break;
		case LVN_ITEMCHANGED:
		{
			if (m_ListViewCtrlData.GetNextItem(-1, LVNI_SELECTED) != -1)
			{
				DWORD dwProcessID = 0;
				_TCHAR tText[MAXBYTE] = { 0 };
				LVITEM lvi = { 0 };
				lvi.pszText = tText;
				lvi.cchTextMax = sizeof(tText) / sizeof(_TCHAR);
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				if (m_ListViewCtrlData.GetSelectedItem(&lvi))
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

	void OnDropFiles(HDROP hDrop)
	{
		std::map<TSTRING, TSTRING> ttmap;
		std::map<TSTRING, TSTRING>::iterator itEnd;
		std::map<TSTRING, TSTRING>::iterator itIdx;

		m_ListViewCtrlData.DeleteAllItems();
		
		if (m_bIsFirst == FALSE)
		{
			UnLoadFile(&m_MapFileStruct);
		}
		else
		{
			m_bIsFirst = FALSE;
		}
		
		GetDropFiles(ttmap, hDrop);
		itEnd = ttmap.end();
		itIdx = ttmap.begin();
		for (; itIdx != itEnd; itIdx++)
		{
			LoadFileR(itIdx->first.c_str(), &m_MapFileStruct);
			if (IsPEFile(m_MapFileStruct.ImageBase) == FALSE)
			{
				UnLoadFile(&m_MapFileStruct);
				MessageBox(_T("������Ч��PE�ļ�"));
				return;
			}
		}
		GetAndShowExports();
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
	
	LRESULT OnSelectAppExe(UINT, int, HWND)
	{
		HANDLE hCFile = NULL;
		DWORD dwWritten = 0;
		CHAR szTemp[0x10000] = { 0 };
		STRINGSTRINGMAPIT itEnd;
		STRINGSTRINGMAPIT itIdx;

		hCFile = CreateFile(m_strCodeFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hCFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(_T("�����ļ�ʧ��"));
			return 0;
		}

		//д��ͷ��
		if (!WriteFile(hCFile, g_pszHeader, strlen(g_pszHeader), &dwWritten, NULL))
		{
			MessageBox(_T("д���ļ�ʧ��"));
			CloseHandle(hCFile);
			return 0;
		}

		//д��#pragma comment����
		// �������Ԫ��
		DWORD dwIndex = 1;
		itEnd = m_FuncListMap.end();
		itIdx = m_FuncListMap.begin();
		for (; itIdx != itEnd; itIdx++)
		{
			ZeroMemory(szTemp, sizeof(szTemp));
			
			sprintf(szTemp, "#pragma comment(linker, \"/EXPORT:%s=_My_%s,@%d\")\r\n", itIdx->first.c_str(), itIdx->first.c_str(), dwIndex);
			dwIndex++;

			//д��#pragma comment����
			if (!WriteFile(hCFile, szTemp, strlen(szTemp), &dwWritten, NULL))
			{
				MessageBox(_T("д���ļ�ʧ��"));
				CloseHandle(hCFile);
				return 0;
			}
		}

		ZeroMemory(szTemp, sizeof(szTemp));
		sprintf(szTemp, g_pszHeader2, dwIndex, "%s");
		dwWritten = 0;
		//����д��
		if (!WriteFile(hCFile, szTemp, strlen(szTemp), &dwWritten, NULL))
		{
			MessageBox(_T("д���ļ�ʧ��"));
			CloseHandle(hCFile);
			return 0;
		}

		DWORD dwCount = 1;
		itEnd = m_FuncListMap.end();
		itIdx = m_FuncListMap.begin();
		for (; itIdx != itEnd; itIdx++)
		{
			ZeroMemory(szTemp, sizeof(szTemp));
			sprintf(szTemp, g_pszFun, dwCount, itIdx->first.c_str(), itIdx->first.c_str(), dwCount, dwCount);
			
			//д��#pragma comment����
			if (!WriteFile(hCFile, szTemp, strlen(g_pszFun), &dwWritten, NULL))
			{
				MessageBox(_T("д���ļ�ʧ��"));
				CloseHandle(hCFile);
				return 0;
			}
			dwCount++;
		}

		CloseHandle(hCFile);
		ZeroMemory(szTemp, sizeof(szTemp));
		sprintf(szTemp, "Դ�����ɳɹ�������·��Ϊ%s", CStringA(m_strCodeFileName));
		MessageBox(CString(szTemp));
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

	/*******************************************************
	*��������:��DLL�л�ȡ�����õ������
	*��������:��
	*��������:��
	*ע������:��
	*����޸�ʱ��:2017/5/16
	*******************************************************/
	BOOL GetAndShowExports()
	{
		CHAR         cBuff[10];
		CHAR		 *szFuncName = NULL;
		UINT                    iNumOfName = 0;
		PDWORD                  pdwRvas, pdwNames;
		PWORD                   pwOrds;
		UINT                    i = 0, j = 0, k = 0;
		BOOL                    bIsByName = FALSE;;


		PIMAGE_NT_HEADERS       pNtH = NULL;
		PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;

		pNtH = GetNtHeaders(m_MapFileStruct.ImageBase);
		if (!pNtH)
			return FALSE;
		pExportDir = (PIMAGE_EXPORT_DIRECTORY)GetExportDirectory(m_MapFileStruct.ImageBase);
		if (!pExportDir)
			return FALSE;


		pwOrds = (PWORD)MyRvaToPtr(pNtH, m_MapFileStruct.ImageBase, pExportDir->AddressOfNameOrdinals);
		pdwRvas = (PDWORD)MyRvaToPtr(pNtH, m_MapFileStruct.ImageBase, pExportDir->AddressOfFunctions);
		pdwNames = (PDWORD)MyRvaToPtr(pNtH, m_MapFileStruct.ImageBase, pExportDir->AddressOfNames);

		if (!pdwRvas)
			return FALSE;


		iNumOfName = pExportDir->NumberOfNames;

		for (i = 0; i<pExportDir->NumberOfFunctions; i++)
		{
			if (*pdwRvas)
			{
				for (j = 0; j<iNumOfName; j++)
				{
					if (i == pwOrds[j])
					{
						bIsByName = TRUE;
						szFuncName = (char*)MyRvaToPtr(pNtH, m_MapFileStruct.ImageBase, pdwNames[j]);
						break;
					}

					bIsByName = FALSE;
				}

				//show funcs to listctrl


				wsprintfA(cBuff, "%04lX", (UINT)(pExportDir->Base + i));

				m_ListViewCtrlData.InsertItem(k, CString(cBuff));

				wsprintfA(cBuff, "%08lX", (*pdwRvas));
				m_ListViewCtrlData.SetItemText(k, 1, CString(cBuff));

				if (bIsByName)
				{
					//m_FunNameList.AddTail(szFuncName);
					m_FuncListMap.insert(STRINGSTRINGMAPPAIR(szFuncName, szFuncName));
					m_ListViewCtrlData.SetItemText(k, 2, CString(szFuncName));
					szFuncName == NULL;
				}
				else
				{
					m_ListViewCtrlData.SetItemText(k, 2, _T("-"));
				}

				//
				++k;

			}

			++pdwRvas;

		}
		return TRUE;
	}
private:
	CString m_strCodeFileName;// = "D:\\MyDll.c";
	STRINGSTRINGMAP m_FuncListMap;

	MAP_FILE_STRUCT m_MapFileStruct;
	BOOL m_bIsFirst;// = TRUE;

	CMenuT<false> m_SystemMenu;
	
	CButtonT<CWindow> m_btnSelectAppExe;
	
	CListViewCtrlT<CWindow> m_ListViewCtrlData;
};
