#include "InputController.h"

InputControllerKeyBoard::InputControllerKeyBoard()
{

}

InputControllerKeyBoard::~InputControllerKeyBoard()
{

}

void InputControllerKeyBoard::Update()
{
	for (int i = 0; i < KEY_MAX; i++)
	{
		if (GetAsyncKeyState(enginInputKeyList[i]))
		{
			inputKeyList[i]++;
		}
		else
		{
			inputKeyList[i] = 0;
		}
	}
}

int InputControllerKeyBoard::GetKeyState(const INPUT_KEY& key)
{
	return inputKeyList[key];
}