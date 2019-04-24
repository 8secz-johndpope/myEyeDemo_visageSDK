// VisionExample.h : main header file for the EXAMPLE7 application
//

#if !defined(AFX_EXAMPLE7_H__C03628EF_3978_42FA_A7F2_152A38008538__INCLUDED_)
#define AFX_EXAMPLE7_H__C03628EF_3978_42FA_A7F2_152A38008538__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVisionExampleApp:
// See VisionExample.cpp for the implementation of this class
//

class CVisionExampleApp : public CWinApp
{
public:
	CVisionExampleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisionExampleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CVisionExampleApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXAMPLE7_H__C03628EF_3978_42FA_A7F2_152A38008538__INCLUDED_)
