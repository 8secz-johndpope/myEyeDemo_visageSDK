// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__03CF625D_5190_4DE4_AA47_DBB6E3C23015__INCLUDED_)
#define AFX_MAINFRM_H__03CF625D_5190_4DE4_AA47_DBB6E3C23015__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpenGLWndFaceTracker.h"
#include "VisionExampleDoc.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
	CVisionExampleDoc* GetDocument();
	COpenGLWnd* GetOpenGLWnd();
	LRESULT OnChangeStatusBar(WPARAM wParam, LPARAM lParam);
	void ShowAvailConfigFiles(char *path,char *currentChoice);
	void ShowAvailCamResolutions();
	void ShowAvailCamDevices();
	void ShowAvailMaxFaces();
	void GetConfigFilename(CString& strName);
	void GetCameraResolution(CString& strName);
	int GetCameraDevice();
	int GetMaxFaces();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CStatusBar  m_wndStatusBar;

	CToolBar	m_wndToolBar;
	CComboBox	*m_comboBoxConfigFile;
	CComboBox	*m_comboBoxCamResolution;
	CComboBox	*m_comboBoxCamDevice;
	CComboBox	*m_comboBoxMaxFaces;
	CReBar		m_wndReBar;
	CDialogBar	m_wndDlgBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG

	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__03CF625D_5190_4DE4_AA47_DBB6E3C23015__INCLUDED_)
