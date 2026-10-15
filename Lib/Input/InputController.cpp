#include "InputController.h"


InputController::InputController()
{
	inputControllerMouse = new InputControllerMouse();
	inputControllerKeyBoard = new InputControllerKeyBoard();
	inputControllerJoyPad = new InputControllerJoyPad();
}

InputController::~InputController()
{
	if (inputControllerMouse != nullptr)
	{
		delete inputControllerMouse;
		inputControllerMouse = nullptr;
	}

	if (inputControllerKeyBoard != nullptr)
	{
		delete inputControllerKeyBoard;
		inputControllerKeyBoard = nullptr;
	}

	if (inputControllerJoyPad != nullptr)
	{
		delete inputControllerJoyPad;
		inputControllerJoyPad = nullptr;
	}

}


void InputController::Update(const HWND& hWnd)
{
	inputControllerMouse->Update(hWnd);
	
	inputControllerKeyBoard->Update();

	inputControllerJoyPad->Update();
}


void InputController::UpdataKeyboard()
{

}

