// InstDrv.h : main header file for the INSTDRV application
//

#if !defined(AFX_INSTDRV_H__D713FF90_275A_4F28_BD5E_A9898D7627E1__INCLUDED_)
#define AFX_INSTDRV_H__D713FF90_275A_4F28_BD5E_A9898D7627E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CInstDrvApp:
// See InstDrv.cpp for the implementation of this class
//

class CInstDrvApp : public CWinApp
{
public:
	CInstDrvApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstDrvApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CInstDrvApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTDRV_H__D713FF90_275A_4F28_BD5E_A9898D7627E1__INCLUDED_)
