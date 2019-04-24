// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VisionExample.h"
#include "VisionExampleDoc.h"
#include "MainFrm.h"

#include "videoInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_MESSAGE(WM_MYMESSAGE , OnChangeStatusBar) //message for displaying the status bar text
	//}}AFX_MSG_MAP

	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_comboBoxConfigFile = NULL;
	m_comboBoxCamResolution = NULL;
	m_comboBoxCamDevice = NULL;
	m_comboBoxMaxFaces = NULL;
}

CMainFrame::~CMainFrame()
{

}

// Returns pointer to the document object. 
// To know more about document object in MFC (Microsoft Foundation Classess) 
// refer to the MSDN documentation.
CVisionExampleDoc* CMainFrame::GetDocument()
{
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
	pos = pDocTemplate->GetFirstDocPosition();
	return (CVisionExampleDoc*)pDocTemplate->GetNextDoc(pos);
}

/**
* Returns pointer to OpenGL rendering window.
*/

COpenGLWnd* CMainFrame::GetOpenGLWnd()
{
	CVisionExampleDoc *pDoc = (CVisionExampleDoc*)GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	return (COpenGLWnd*)pDoc->GetNextView(pos);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_AUTOSIZE,
			WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP /*| CBRS_TOOLTIPS*/ | CBRS_FLYBY) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndDlgBar.Create(this, IDD_DIALOGBAR, CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_DIALOGBAR) ||
		!m_wndReBar.AddBar(&m_wndDlgBar) ||
		!m_wndReBar.AddBar(&m_wndToolBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_comboBoxConfigFile = (CComboBox *)m_wndDlgBar.GetDlgItem(ID_COMBO_CONFIG_FILE);
	m_comboBoxCamResolution = (CComboBox *)m_wndDlgBar.GetDlgItem(ID_COMBO_CAM_RESOLUTION);
	m_comboBoxCamDevice = (CComboBox *)m_wndDlgBar.GetDlgItem(ID_COMBO_CAM_DEVICE);
	m_comboBoxMaxFaces = (CComboBox *)m_wndDlgBar.GetDlgItem(ID_COMBO_MAX_FACES);


	return 0;
}

BOOL CMainFrame::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXT);
	
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	TCHAR szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if (pTTT->uFlags & TTF_IDISHWND)
	{
		// idFrom is actually the HWND of the tool 
		nID = ::GetDlgCtrlID((HWND)nID);
	}

	if (nID != 0) // will be zero on a separator
	{
		strTipText.LoadString(nID);
		lstrcpyn(pTTT->szText, strTipText, sizeof(pTTT->szText));

		*pResult = 0;

		// bring the tooltip window above other popup windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);

		return TRUE;
	}

	return FALSE;
}

//window size x+20 y+110
#define xsize 1100
#define ysize 720

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx=xsize;
	cs.cy=ysize;
	/*
    cs.y = ::GetSystemMetrics(SM_CYSCREEN) - ysize - (::GetSystemMetrics(SM_CYSCREEN) / 30); 
	if(cs.y < 0 ) cs.y = 0;
    cs.x = 0; 
	*/
	cs.x = (::GetSystemMetrics(SM_CXSCREEN) - xsize) / 2; 
	cs.y = (::GetSystemMetrics(SM_CYSCREEN) - ysize) / 2;

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}
LRESULT CMainFrame::OnChangeStatusBar(WPARAM wParam, LPARAM lParam)
{
	if(wParam != NULL)
	{
		CString* strStatusText = (CString*) wParam;
		//Update the statusbar
		if (m_wndStatusBar) m_wndStatusBar.SetPaneText( 0, *strStatusText, TRUE );
		//free memory
		delete strStatusText;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::ShowAvailConfigFiles(char *path,char *currentChoice)
{
	HANDLE hSearch;
	WIN32_FIND_DATA FileData;
	CString dupa;
	CString temp;

	temp = currentChoice;

	while (m_comboBoxConfigFile->GetCount() >0) m_comboBoxConfigFile->DeleteString(0);
	
	dupa.Format("%s", path);
	if (dupa == "") return;	
	dupa += "\\*.cfg";
	hSearch = FindFirstFile(dupa, &FileData); 

	if (hSearch == INVALID_HANDLE_VALUE){
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
			);
		// Free the buffer.
		LocalFree( lpMsgBuf );
	} 
	int iIndex = 0;

	do {
		if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
				m_comboBoxConfigFile->InsertString(iIndex, FileData.cFileName);
				if (!temp.CompareNoCase(FileData.cFileName)) 
					m_comboBoxConfigFile->SetCurSel(iIndex);
				iIndex++;
		}
	}while (FindNextFile(hSearch, &FileData));
	m_comboBoxConfigFile->EnableWindow(m_comboBoxConfigFile->GetCount());
}

void CMainFrame::ShowAvailCamResolutions()
{
	while (m_comboBoxCamResolution->GetCount() > 0) m_comboBoxCamResolution->DeleteString(0);

	int iIndex = 0;

	m_comboBoxCamResolution->InsertString(iIndex++, "320x240");
	m_comboBoxCamResolution->InsertString(iIndex++, "640x480");
	m_comboBoxCamResolution->InsertString(iIndex++, "800x600");
	m_comboBoxCamResolution->InsertString(iIndex++, "1280x720");
	m_comboBoxCamResolution->InsertString(iIndex++, "1920x1080");
	m_comboBoxCamResolution->InsertString(iIndex++, "2560x1440");
	m_comboBoxCamResolution->InsertString(iIndex++, "4096x2160");

	m_comboBoxCamResolution->SetCurSel(2);

	m_comboBoxCamResolution->EnableWindow(m_comboBoxCamResolution->GetCount());
}

void CMainFrame::ShowAvailCamDevices()
{
	while (m_comboBoxCamDevice->GetCount() > 0) m_comboBoxCamDevice->DeleteString(0);

	int n = videoInput::listDevices();

	for (int i = 0; i < n; ++i) {
		m_comboBoxCamDevice->InsertString(i, videoInput::getDeviceName(i));
	}

	m_comboBoxCamDevice->SetCurSel(0);

	m_comboBoxCamDevice->EnableWindow(m_comboBoxCamDevice->GetCount());
}

void CMainFrame::ShowAvailMaxFaces()
{
	while (m_comboBoxMaxFaces->GetCount() > 0) m_comboBoxMaxFaces->DeleteString(0);

	for (int i = 0; i < 10; i++)
		m_comboBoxMaxFaces->InsertString(i, std::to_string(i+1).c_str());

	m_comboBoxMaxFaces->SetCurSel(2);

	m_comboBoxMaxFaces->EnableWindow(m_comboBoxMaxFaces->GetCount());
}

void CMainFrame::GetConfigFilename(CString& strName)
{
	m_comboBoxConfigFile = (CComboBox *)m_wndDlgBar.GetDlgItem(ID_COMBO_CONFIG_FILE);
	m_comboBoxConfigFile->GetLBText(m_comboBoxConfigFile->GetCurSel(), strName);
}

void CMainFrame::GetCameraResolution(CString& strName)
{
	m_comboBoxCamResolution = (CComboBox *)m_wndDlgBar.GetDlgItem(ID_COMBO_CAM_RESOLUTION);
	m_comboBoxCamResolution->GetLBText(m_comboBoxCamResolution->GetCurSel(), strName);
}

int CMainFrame::GetCameraDevice()
{
	m_comboBoxCamDevice = (CComboBox *)m_wndDlgBar.GetDlgItem(ID_COMBO_CAM_DEVICE);
	return m_comboBoxCamDevice ? m_comboBoxCamDevice->GetCurSel() : 0;
}

int CMainFrame::GetMaxFaces()
{
	m_comboBoxMaxFaces = (CComboBox *)m_wndDlgBar.GetDlgItem(ID_COMBO_MAX_FACES);
	return m_comboBoxMaxFaces ? m_comboBoxMaxFaces->GetCurSel() + 1 : 1;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

