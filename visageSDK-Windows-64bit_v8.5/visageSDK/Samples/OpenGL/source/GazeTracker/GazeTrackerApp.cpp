// Example7.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GazeTrackerApp.h"
#include "FolderManager.h"
#include "windows.h"
#include "resource.h"
#include "GazeTrackerDoc.h"
#include <tchar.h>

GazeTrackerDoc* gazeTracker;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExample7App

BEGIN_MESSAGE_MAP(GazeTrackerApp, CWinApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExample7App construction


GazeTrackerApp::GazeTrackerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CExample7App object

GazeTrackerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CExample7App initialization

BOOL GazeTrackerApp::InitInstance()
{
	visageFolderManager.init();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	//gazeTracker = new GazeTrackerDoc();
	//gazeTracker->StartGazeTracking();

	gazeTracker = new GazeTrackerDoc();

	gazeTracker->StartGazeTracking();

	return TRUE;
}