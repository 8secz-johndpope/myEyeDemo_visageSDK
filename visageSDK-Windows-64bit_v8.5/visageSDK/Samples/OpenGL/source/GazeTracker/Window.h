#include <windows.h>
#include <tchar.h>
#include "IMouseHandler.h"

class Window
{
public :
	void Init(int width, int height, int bits, wchar_t* name);
	bool ProcessWinMsg();
	void SetRenderingContext();
	void Show();
	void Swap();
	
	HWND hWnd;
	MSG Msg;

	bool recalibStarted;
	bool isClicked;
	bool exit;

	IMouseHandler* mouseHandler;

private:
	HDC hDC;
};