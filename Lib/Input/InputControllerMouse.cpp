#include "InputController.h"

InputControllerMouse::InputControllerMouse()
{

}


InputControllerMouse::~InputControllerMouse()
{

}

void InputControllerMouse::Update(const HWND& hWnd)
{
	//ウィンドウの位置を取得
	WINDOWINFO windowInfo;
	windowInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(hWnd, &windowInfo);

	GetCursorPos(&pt);

	pt.x += windowInfo.rcWindow.left;
	pt.y += windowInfo.rcWindow.top;
}