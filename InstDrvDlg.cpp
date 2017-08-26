// InstDrvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InstDrv.h"
#include "InstDrvDlg.h"

#include "Global.h"

#include <setupapi.h>
#include <newdev.h>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "newdev.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstDrvDlg dialog

CInstDrvDlg::CInstDrvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstDrvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstDrvDlg)
	m_strSysFilePath = _T("");
	m_nSysType = NT_SYS;
	m_bTopAlways = TRUE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strSysFileName = _T("");
	m_hSerMgr = NULL;
	m_hServer = NULL;
	m_nSysStartType = DEMAND_START;
	m_strHardwareID = _T("");
	m_bUpdateStartType = FALSE;
}

void CInstDrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstDrvDlg)
	DDX_Text(pDX, IDC_SYS_PATH_EDIT, m_strSysFilePath);
	DDX_Radio(pDX, IDC_NT_SYS_RADIO, m_nSysType);
	DDX_Check(pDX, IDC_TOP_CHECK, m_bTopAlways);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstDrvDlg, CDialog)
	//{{AFX_MSG_MAP(CInstDrvDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BROWSE_BTN, OnBrowseBtn)
	ON_BN_CLICKED(IDC_TOP_CHECK, OnTopCheck)
	ON_BN_CLICKED(IDC_WDM_SYS_RADIO, OnWdmSysRadio)
	ON_BN_CLICKED(IDC_NT_SYS_RADIO, OnNtSysRadio)
	ON_BN_CLICKED(IDC_LOAD_BTN, OnLoadBtn)
	ON_BN_CLICKED(IDC_START_BTN, OnStartBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, OnStopBtn)
	ON_BN_CLICKED(IDC_UNLOAD_BTN, OnUnloadBtn)
	ON_BN_CLICKED(IDC_START_SYS_CONFIG_BTN, OnStartSysConfigBtn)
	ON_COMMAND(ID_AUTO_START_MENUITEM, OnAutoStartMenuitem)
	ON_COMMAND(ID_BOOT_START_MENUITEM, OnBootStartMenuitem)
	ON_COMMAND(ID_DEMAND_START_MENUITEM, OnDemandStartMenuitem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstDrvDlg message handlers


BOOL ChangeWndMessageFilter(UINT nMessage, BOOL bAllow)
{
	typedef BOOL (WINAPI * ChangeWindowMessageFilterOkFn)(UINT, DWORD);

	HMODULE hModUser32 = NULL;

	hModUser32 = LoadLibrary(_T("user32.dll"));

	if (hModUser32 == NULL) {
		return FALSE;
	}

	ChangeWindowMessageFilterOkFn pfnChangeWindowMessageFilter = (ChangeWindowMessageFilterOkFn) GetProcAddress(hModUser32, "ChangeWindowMessageFilter");

	if (pfnChangeWindowMessageFilter == NULL)
	{
		FreeLibrary(hModUser32);
		return FALSE;
	}
	FreeLibrary(hModUser32);
	return pfnChangeWindowMessageFilter(nMessage, bAllow ? MSGFLT_ADD : MSGFLT_REMOVE);
}

BOOL CInstDrvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

	ChangeWndMessageFilter(WM_DROPFILES, TRUE);
	ChangeWndMessageFilter(0x0049, TRUE); // 0x0049 - WM_COPYGLOBALDATA
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstDrvDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInstDrvDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInstDrvDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CInstDrvDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	if (NULL != m_hServer)
	{
		CloseServiceHandle(m_hServer);
		m_hServer = NULL;
	}
	if (NULL != m_hSerMgr)
	{
		CloseServiceHandle(m_hSerMgr);
		m_hSerMgr = NULL;
	}
	UINT nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (nFiles > 1)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：只能一次拖曳一个文件！"));
	}
	else
	{
		TCHAR pFileBuf[MAX_PATH + 1] = {0};
		DragQueryFile(hDropInfo, 0, pFileBuf, MAX_PATH * sizeof(TCHAR));
		m_strSysFilePath = pFileBuf;
		int index = m_strSysFilePath.ReverseFind('\\');
		m_strSysFileName = m_strSysFilePath.Right(m_strSysFilePath.GetLength() - index - 1);
		UpdateData(FALSE);
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：拖曳获取文件路径成功！"));
		if (m_nSysType == WDM_SYS)
		{
			ParseInfFile(m_strSysFilePath);
		}
	}
	DragFinish(hDropInfo);
}

void CInstDrvDlg::OnBrowseBtn() 
{
	// TODO: Add your control notification handler code here
	if (NULL != m_hServer)
	{
		CloseServiceHandle(m_hServer);
		m_hServer = NULL;
	}
	if (NULL != m_hSerMgr)
	{
		CloseServiceHandle(m_hSerMgr);
		m_hSerMgr = NULL;
	}
	UpdateData(TRUE);
	CString strFilter = "";
	if (m_nSysType == NT_SYS)
	{
		strFilter = _T("sys file (*.sys)|*.sys||");
	}
	else
	{
		strFilter = _T("inf file (*.inf)|*.inf||");
	}
	CFileDialog openFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);
	if (openFileDlg.DoModal() == IDOK)
	{
		m_strSysFilePath = openFileDlg.GetPathName();
		m_strSysFileName = openFileDlg.GetFileName();
		UpdateData(FALSE);
		if (m_nSysType == WDM_SYS)
		{
			ParseInfFile(m_strSysFilePath);
		}
	}
}

void CInstDrvDlg::OnTopCheck() 
{
	// TODO: Add your control notification handler code here
	m_bTopAlways = !m_bTopAlways;
	if (m_bTopAlways)
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
}

void CInstDrvDlg::OnWdmSysRadio() 
{
	// TODO: Add your control notification handler code here
	if (NULL != m_hServer)
	{
		CloseServiceHandle(m_hServer);
		m_hServer = NULL;
	}
	if (NULL != m_hSerMgr)
	{
		CloseServiceHandle(m_hSerMgr);
		m_hSerMgr = NULL;
	}
	m_nSysType = WDM_SYS;
	m_strSysFilePath = _T("");
	GetDlgItem(IDC_START_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP_BTN)->EnableWindow(FALSE);
	GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：WDM驱动请选择Inf配置文件！"));
}

void CInstDrvDlg::OnNtSysRadio() 
{
	// TODO: Add your control notification handler code here
	if (NULL != m_hServer)
	{
		CloseServiceHandle(m_hServer);
		m_hServer = NULL;
	}
	if (NULL != m_hSerMgr)
	{
		CloseServiceHandle(m_hSerMgr);
		m_hSerMgr = NULL;
	}
	m_nSysType = NT_SYS;
	m_strSysFilePath = _T("");
	GetDlgItem(IDC_START_BTN)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP_BTN)->EnableWindow(TRUE);
	GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：NT驱动请选择sys驱动文件！"));
}

void CInstDrvDlg::OnLoadBtn() 
{
	// TODO: Add your control notification handler code here
	if (m_nSysType == NT_SYS)
	{
		LoadNtSys(m_nSysStartType);
	}
	else
	{
		LoadWDMSys();
	}
}

void CInstDrvDlg::OnStartBtn() 
{
	// TODO: Add your control notification handler code here
	if (m_nSysType == NT_SYS)
	{
		StartNTSys();
	}
}

void CInstDrvDlg::OnStopBtn() 
{
	// TODO: Add your control notification handler code here
	if (m_nSysType == NT_SYS)
	{
		StopNtSys();
	}
}

void CInstDrvDlg::OnUnloadBtn() 
{
	// TODO: Add your control notification handler code here
	if (m_nSysType == NT_SYS)
	{
		UnloadNtSys();
	}
	else
	{
		UnloadWDMSys();
	}
}

void CInstDrvDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (NULL != m_hServer)
	{
		CloseServiceHandle(m_hServer);
		m_hServer = NULL;
	}
	if (NULL != m_hSerMgr)
	{
		CloseServiceHandle(m_hSerMgr);
		m_hSerMgr = NULL;
	}
	CDialog::OnCancel();
}

void CInstDrvDlg::CloseServerHandle()
{
	if (NULL != m_hServer)
	{
		CloseServiceHandle(m_hServer);
		m_hServer = NULL;
	}
	if (NULL != m_hSerMgr)
	{
		CloseServiceHandle(m_hSerMgr);
		m_hSerMgr = NULL;
	}
}

void CInstDrvDlg::LoadNtSys(UINT nSysStartType /* = DEMAND_START */)
{
	if (NULL != m_hSerMgr)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：因为驱动服务已经存在，所以驱动服务安装失败！"));
		return;
	}
	HANDLE hToken;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);	
	AdjustProcessPrivilege(hToken, SE_DEBUG_NAME);
	m_hSerMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == m_hSerMgr)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动服务安装失败！"));
		return;
	}
	DWORD dwStartType;
	switch(nSysStartType)
	{
	case BOOT_START:
		dwStartType = SERVICE_BOOT_START;
		break;

	case AUTO_START:
		dwStartType = SERVICE_AUTO_START;
		break;

	case DEMAND_START:
	default:
		dwStartType = SERVICE_DEMAND_START;
		break;
	}
	m_hServer = CreateService(m_hSerMgr, m_strSysFileName, m_strSysFileName, SERVICE_ALL_ACCESS, \
		SERVICE_KERNEL_DRIVER, dwStartType, SERVICE_ERROR_NORMAL, m_strSysFilePath, NULL, NULL, \
		NULL, NULL, NULL);
	if (NULL == m_hServer)
	{
		DWORD dwRet = GetLastError();
		if (dwRet != ERROR_IO_PENDING && dwRet != ERROR_SERVICE_EXISTS)
		{
			GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动服务安装失败！"));
			if (NULL != m_hSerMgr)
			{
				CloseServiceHandle(m_hSerMgr);
				m_hSerMgr = NULL;
			}
			return;
		}
		m_hServer = OpenService(m_hSerMgr, m_strSysFileName, SERVICE_ALL_ACCESS);
		if (NULL == m_hServer)
		{
			GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：因为驱动服务已经存在，所以驱动服务安装失败！"));
			return;
		}
	}
	GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动服务安装成功！"));
}

void CInstDrvDlg::StartNTSys()
{
	if (NULL == m_hSerMgr && NULL == m_hServer)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：启动驱动失败！"));
		return;
	}
	if (!StartService(m_hServer, NULL, NULL))
	{
		DWORD dwRet = GetLastError();
		if (dwRet != ERROR_IO_PENDING && dwRet != ERROR_SERVICE_ALREADY_RUNNING)
		{
			GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：启动驱动失败！"));
			return;
		}
		else
		{
			GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：因为驱动服务已经启动，所以驱动服务启动失败！"));
			return;
		}
	}
	GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动服务启动成功！"));
}

void CInstDrvDlg::StopNtSys()
{
	if (NULL == m_hSerMgr && NULL == m_hServer)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：因为驱动服务没有启动，所以驱动服务停止失败！"));
		return;
	}
	SERVICE_STATUS stSerStatus = {0};
	if (!ControlService(m_hServer, SERVICE_CONTROL_STOP, &stSerStatus))
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：因为驱动服务没有启动，所以驱动服务停止失败！"));
		return;
	}
	GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动服务停止成功！"));
}

void CInstDrvDlg::UnloadNtSys()
{
	if (NULL == m_hSerMgr && NULL == m_hServer)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：打开驱动服务失败，可能该驱动服务不存在！"));
		return;
	}
	if (!DeleteService(m_hServer))
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动服务卸载失败！"));
		return;
	}
	CloseServerHandle();
	GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动服务卸载成功！"));
}

void CInstDrvDlg::OnStartSysConfigBtn() 
{
	// TODO: Add your control notification handler code here
	CMenu externMenu;
	externMenu.LoadMenu(IDR_MENU);
	CMenu* popMenu = externMenu.GetSubMenu(0);
	CRect rcBtnClient;
	GetDlgItem(IDC_START_SYS_CONFIG_BTN)->GetWindowRect(&rcBtnClient);
	switch(m_nSysStartType)
	{
	case BOOT_START:
		popMenu->CheckMenuItem(ID_BOOT_START_MENUITEM, MF_CHECKED);
		break;

	case AUTO_START:
		popMenu->CheckMenuItem(ID_AUTO_START_MENUITEM, MF_CHECKED);
		break;

	case DEMAND_START:
	default:
		popMenu->CheckMenuItem(ID_DEMAND_START_MENUITEM, MF_CHECKED);
		break;
	}
	popMenu->TrackPopupMenu(TPM_LEFTALIGN, rcBtnClient.left + rcBtnClient.Width(), \
		rcBtnClient.top + rcBtnClient.Height(), this);
}

void CInstDrvDlg::LoadWDMSys()
{
    InstallWDMSys(m_strSysFilePath);
}

void CInstDrvDlg::InstallWDMSys(LPCTSTR lpStrInfFileName)
{
	GUID guid = {0};
	SP_DEVINFO_DATA stSPDevData = {0};
	HDEVINFO hDevInfo = NULL;
	TCHAR className[32] = {0};
	BOOL bRebootRequired = FALSE;
	 
	if (!SetupDiGetINFClass(lpStrInfFileName, &guid, className, 32, 0))
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动安装失败！"));
		return;
	}
	 
	hDevInfo = SetupDiCreateDeviceInfoList(&guid, this->m_hWnd);
	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动安装失败！"));
		return;
	}
	 
	stSPDevData.cbSize = sizeof(SP_DEVINFO_DATA);
	if (!SetupDiCreateDeviceInfo(hDevInfo, className, &guid, NULL, this->m_hWnd, DICD_GENERATE_ID, \
		&stSPDevData))
	{
		SetupDiDestroyDeviceInfoList(hDevInfo);
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动安装失败！"));
		return;
	}
	 
	DWORD nSize = (m_strHardwareID.GetLength() + 2) * sizeof(TCHAR);
	PBYTE pHardwareid = new BYTE[nSize];
	if (NULL == pHardwareid)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动安装失败！"));
		return;
	}
	memset(pHardwareid, 0, nSize);
	memcpy(pHardwareid, (LPCTSTR) m_strHardwareID, nSize - 2 * sizeof(TCHAR));
	
	if (!SetupDiSetDeviceRegistryProperty(hDevInfo, &stSPDevData, SPDRP_HARDWAREID, pHardwareid, nSize))
	{
		if (NULL != pHardwareid)
		{
			delete[] pHardwareid;
			pHardwareid = NULL;
		}
		SetupDiDestroyDeviceInfoList(hDevInfo);
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动安装失败！"));
		return;
	}

	if (NULL != pHardwareid)
	{
		delete[] pHardwareid;
		pHardwareid = NULL;
	}
	 
	if (!SetupDiCallClassInstaller(DIF_REGISTERDEVICE, hDevInfo, &stSPDevData))
	{
		SetupDiDestroyDeviceInfoList(hDevInfo);
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动安装失败！"));
		return;
	}
	 
	if (!UpdateDriverForPlugAndPlayDevices(this->m_hWnd, m_strHardwareID, lpStrInfFileName, \
		INSTALLFLAG_FORCE, &bRebootRequired))
	{
		SetupDiDestroyDeviceInfoList(hDevInfo);
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动安装失败！"));
		return;
	}
	
	SetupDiDestroyDeviceInfoList(hDevInfo);
	GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动安装成功！"));
}

void CInstDrvDlg::UnloadWDMSys()
{
	SP_DEVINFO_DATA stSPDevInfoData = {0};
	HDEVINFO hDevInfo = NULL;
	WORD wIdx, wCount = 0;
	 
	hDevInfo = SetupDiGetClassDevs(NULL, NULL, this->m_hWnd, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动暂时无法卸载！"));
		return;
	}
	 
	wIdx = 0;
	while (TRUE)
	{
		stSPDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		if (SetupDiEnumDeviceInfo(hDevInfo, wIdx, &stSPDevInfoData))
		{
			TCHAR buffer[32] = {0};
	 
			if (SetupDiGetDeviceRegistryProperty(hDevInfo, &stSPDevInfoData, SPDRP_HARDWAREID, \
				NULL, (PBYTE)buffer, 32, NULL))
			{
				if (!_tcscmp(m_strHardwareID, (LPTSTR)buffer))
				{
					if (!SetupDiRemoveDevice(hDevInfo, &stSPDevInfoData))
					{
						GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动暂时无法卸载！"));
					}
					wCount++;
				}
			}
		}
		else
		{
			break;
		}		
		wIdx++;
	}
	 
	if (wCount != 0)
	{
		GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(_T("状态：驱动已成功卸载！"));
	}
	 
	SetupDiDestroyDeviceInfoList(hDevInfo);
}

void CInstDrvDlg::OnAutoStartMenuitem() 
{
	// TODO: Add your command handler code here
	if (m_nSysStartType == AUTO_START)
	{
		m_bUpdateStartType = FALSE;
		return;
	}
	m_nSysStartType = AUTO_START;
	m_bUpdateStartType = TRUE;
	ChangeServerStartConfig();
}

void CInstDrvDlg::OnBootStartMenuitem() 
{
	// TODO: Add your command handler code here
	if (m_nSysStartType == BOOT_START)
	{
		m_bUpdateStartType = FALSE;
		return;
	}
	m_nSysStartType = BOOT_START;
	m_bUpdateStartType = TRUE;
	ChangeServerStartConfig();
}

void CInstDrvDlg::OnDemandStartMenuitem() 
{
	// TODO: Add your command handler code here
	if (m_nSysStartType == DEMAND_START)
	{
		m_bUpdateStartType = FALSE;
		return;
	}
	m_nSysStartType = DEMAND_START;
	m_bUpdateStartType = TRUE;
	ChangeServerStartConfig();
}

void CInstDrvDlg::ChangeServerStartConfig()
{
	if (!m_bUpdateStartType)
	{
		return;
	}
	CString strStartType;
	UINT nStartType = 3;
	switch(m_nSysStartType)
	{
	case BOOT_START:
		nStartType = SERVICE_BOOT_START;
		strStartType.Format(_T("状态：驱动启动类型更改为引导启动！"));
		break;
		
	case AUTO_START:
		nStartType = SERVICE_AUTO_START;
		strStartType.Format(_T("状态：驱动启动类型更改为开机启动！"));
		break;
		
	case DEMAND_START:
	default:
		nStartType = SERVICE_DEMAND_START;
		strStartType.Format(_T("状态：驱动启动类型更改为加载启动！"));
		break;
	}
	if (m_nSysType == WDM_SYS)
	{
		m_infParser.SetValueToStartTypeField(nStartType);
	}
	GetDlgItem(IDC_ALERT_STATIC)->SetWindowText(strStartType);
}

void CInstDrvDlg::ParseInfFile(LPCTSTR lpStrInfFileName)
{
	m_infParser.Open(lpStrInfFileName);
	if (m_infParser.Parse())
	{
		vector<CString> strField = m_infParser.GetFiledFromSection("Manufacturer");
		CString strHardwareValue = m_infParser.GetValueStringFromField(strField[0]);
		vector<CString> arValue;
		m_infParser.SplitStrByChEx(strHardwareValue, ',', arValue);
		int nValueCount = arValue.size();
		if (nValueCount == 1)
		{
			vector<CString> strHardwareField = m_infParser.GetFiledFromSection(strHardwareValue);
			CString strHardwareIDValue = m_infParser.GetValueStringFromField(strHardwareField[0]);
			vector<CString> strHardwareID;
			m_infParser.SplitStrByCh(strHardwareIDValue, ',', strHardwareID);
			if (strHardwareID.size() > 1)
			{
				m_strHardwareID = strHardwareID[1];
			}
		}
		else
		{
			CString strHardwareSec = arValue[0] + "." + arValue[1];
			vector<CString> strHardwareField = m_infParser.GetFiledFromSection(strHardwareSec);
			CString strHardwareIDValue = m_infParser.GetValueStringFromField(strHardwareField[0]);
			vector<CString> strHardwareID;
			m_infParser.SplitStrByCh(strHardwareIDValue, ',', strHardwareID);
			if (strHardwareID.size() > 1)
			{
				m_strHardwareID = strHardwareID[1];
			}
		}
	}
	m_infParser.Close();
}
