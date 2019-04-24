#include "windows.h"
#include "resource.h"
#include "GazeTrackerDoc.h"
#include <tchar.h>

GazeTrackerDoc* gazeTracker;

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int curTimestamp = 0;
    switch(Message)
    {
        case WM_INITDIALOG:
        return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_RECALIB:
					gazeTracker->observer->RecalibStarted();
					break;
            }
			 break;
		case WM_CLOSE:
			EndDialog(hwnd, 0);
			exit(1);
			break;   
        default:	
			HWND dlg = GetDlgItem(hwnd, IDC_TRACK_STAT);
	
			switch(gazeTracker->observer->track_stat)
			{
				case 1:
					EnableWindow(GetDlgItem(hwnd, ID_RECALIB), TRUE);
					SetWindowText(dlg, _T("OK"));
					break;
				case 2:
					EnableWindow(GetDlgItem(hwnd, ID_RECALIB), FALSE);
					SetWindowText(dlg, _T("RECOVERING"));
					break;
				case 3:
					EnableWindow(GetDlgItem(hwnd, ID_RECALIB), FALSE);
					SetWindowText(dlg, _T("INITIALIZING"));
					break;
				default:
					EnableWindow(GetDlgItem(hwnd, ID_RECALIB), FALSE);
					SetWindowText(dlg, _T("OFF"));
					break;
			}
            return FALSE;
    }
    return TRUE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	gazeTracker = new GazeTrackerDoc();

	Window* window = new Window();
	window->Init(240, 120, 16, _T("MainWin"));

	gazeTracker->StartGazeTracking();

	//DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), window->hWnd, AboutDlgProc);

	bool done = false;

	while(!done)
	{
		window->ProcessWinMsg();
		Sleep(1000);
	}

	return window->Msg.wParam;
}