#include "Window.h"
#include "glut.h"


bool keys[256];	
bool recalib = false;
bool isClicked = false;

#define VK_E 0x45
#define VK_W 0x57
#define VK_S 0x53

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	isClicked = false;
	bool exit = false;

    switch(msg)
    {
		case WM_NCCREATE: 
		{
            CREATESTRUCT * pcs = (CREATESTRUCT*)lParam;
            Window* wnd = (Window*)pcs->lpCreateParams;
            SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams ); 
            return TRUE;
        }

        case WM_CLOSE:
			exit = true;
            DestroyWindow(hwnd);
			break;
        case WM_DESTROY:
            PostQuitMessage(0);
			break;
		
		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			break;							// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			break;								// Jump Back
		}
		case WM_LBUTTONDOWN:
		{
			isClicked = true;
			break;
		}
		case WM_LBUTTONUP:
		{
			isClicked = false;
			break;
		}
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
	
	Window* wnd = (Window*)GetWindowLongPtr( hwnd, GWLP_USERDATA );

	wnd->isClicked = false;
	wnd->recalibStarted = false;
	wnd->exit = exit;

	if((keys[VK_ESCAPE]))
	{	
		wnd->exit = true;
	}

	if(keys[VK_SPACE])
	{
		wnd->recalibStarted = true;
	}


	if(isClicked)
	{
		wnd->isClicked = true;

		POINT p;
		GetCursorPos(&p);

		wnd->mouseHandler->OnLeftButtonDown(p.x, p.y);
	}

    return 0;
}


void Window::Init(int width, int height, int bits, wchar_t* name)
{
	WNDCLASSEX wc;
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;	
	RECT		WindowRect;	

	wchar_t* g_szClassName = name;

    //Step 1: Registering the Window Class
	HINSTANCE hInstance = GetModuleHandle(0);
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, _T("Window Registration Failed!"), _T("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return;
    }

	DEVMODE dmScreenSettings;								// Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
	dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
	dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{

	}

	dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
	dwStyle=WS_OVERLAPPEDWINDOW;									// Windows Style
	ShowCursor(TRUE);	

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	

    // Step 2: Creating the Window
    hWnd = CreateWindowEx(
        dwExStyle,
        g_szClassName,
        _T("Gaze Tracker"),
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, hInstance, this);

    if(hWnd == NULL)
    {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return;
    }
}

bool Window::ProcessWinMsg()
{
	if (PeekMessage(&Msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
	{
		TranslateMessage(&Msg);				// Translate The Message
		DispatchMessage(&Msg);				// Dispatch The Message
		
		return true;
	}
	else
	{
		return false;
	}
}

void Window::Show()
{
	ShowWindow(hWnd, SW_SHOW);
}

void Window::Swap()
{
	SwapBuffers(hDC);
}

void Window::SetRenderingContext()
{
	if(!(hDC = GetDC(hWnd)))
	{
		MessageBox(NULL, _T(""), _T("ERROR"), MB_OK);
	}

	//api switch - 4. parametar, color depth switch 7. - parametar
	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		16,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(!SetPixelFormat(hDC, pixelFormat, &pfd)) 
	{
		MessageBox(NULL, _T(""), _T("ERROR"), MB_OK);
	}

	HGLRC hRC = 0;

	if(!(hRC = wglCreateContext(hDC)))
	{
		MessageBox(NULL, _T(""), _T("ERROR"), MB_OK);
	}
	
	if(!wglMakeCurrent(hDC, hRC))
	{
		MessageBox(NULL, _T(""), _T("ERROR"), MB_OK);
	}
}