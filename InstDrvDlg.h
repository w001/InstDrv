// InstDrvDlg.h : header file
//

#if !defined(AFX_INSTDRVDLG_H__F6C0A3DA_B935_4086_AF24_75100B083626__INCLUDED_)
#define AFX_INSTDRVDLG_H__F6C0A3DA_B935_4086_AF24_75100B083626__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsvc.h>
#include "InfParser.h"

/////////////////////////////////////////////////////////////////////////////
// CInstDrvDlg dialog

class CInstDrvDlg : public CDialog
{
// Construction
public:
	CInstDrvDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstDrvDlg)
	enum { IDD = IDD_INSTDRV_DIALOG };
	CString	m_strSysFilePath;
	int		m_nSysType;
	BOOL	m_bTopAlways;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstDrvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CInstDrvDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBrowseBtn();
	afx_msg void OnTopCheck();
	afx_msg void OnWdmSysRadio();
	afx_msg void OnNtSysRadio();
	afx_msg void OnLoadBtn();
	afx_msg void OnStartBtn();
	afx_msg void OnStopBtn();
	afx_msg void OnUnloadBtn();
	virtual void OnCancel();
	afx_msg void OnStartSysConfigBtn();
	afx_msg void OnAutoStartMenuitem();
	afx_msg void OnBootStartMenuitem();
	afx_msg void OnDemandStartMenuitem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	enum _SYS_TYPE_
	{
		NT_SYS = 0,
		WDM_SYS
	};

	enum _SYS_START_TYPE_
	{
		BOOT_START = 0,
		AUTO_START,
		DEMAND_START
	};

	CString m_strSysFileName;
	SC_HANDLE m_hSerMgr;
	SC_HANDLE m_hServer;
	UINT m_nSysStartType;
	CInfParser m_infParser;
	CString m_strHardwareID;
	BOOL m_bUpdateStartType;

private:
	void CloseServerHandle();
	void LoadNtSys(UINT nSysStartType = DEMAND_START);
	void StartNTSys();
	void StopNtSys();
	void UnloadNtSys();

	void LoadWDMSys();
	void InstallWDMSys(LPCTSTR lpStrInfFileName);
	void UnloadWDMSys();

	void ChangeServerStartConfig();
	void ParseInfFile(LPCTSTR lpStrInfFileName);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTDRVDLG_H__F6C0A3DA_B935_4086_AF24_75100B083626__INCLUDED_)
