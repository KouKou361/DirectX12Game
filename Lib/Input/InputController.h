#pragma once

#include <Windows.h>
#include <math.h>

enum INPUT_KEY
{
	MOUSE_RIGHT,
	MOUSE_LEFT,

	KEY_ENTER,
	KEY_SHIFT,
	KEY_CTRL,
	KEY_SPACE,
	KEY_ESAPE,

	KEY_W,
	KEY_A,
	KEY_S,
	KEY_D,

	KEY_UP,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_LEFT,

	KEY_MAX
};
static BYTE enginInputKeyList[KEY_MAX] =
{
	VK_RBUTTON,
	VK_LBUTTON,

	VK_RETURN,
	VK_SHIFT,
	VK_CONTROL,
	VK_PAUSE,
	VK_ESCAPE,

	'W',
	'A',
	'S',
	'D',

	VK_UP,
	VK_DOWN,
	VK_RIGHT,
	VK_LEFT,




};

//マウス専用
class InputControllerMouse
{
public:
	InputControllerMouse();
	~InputControllerMouse();

	void Update(const HWND& hWnd);

	POINT GetMousePos() { return pt; };
	void SetMousePos(const int& x, const int& y) { SetCursorPos(x, y); };

private:
	POINT pt;
};


//キーボード専用
class InputControllerKeyBoard
{
public:
	InputControllerKeyBoard();
	~InputControllerKeyBoard();

	void Update();

	int GetKeyState(const INPUT_KEY& key);

private:
	int inputKeyList[KEY_MAX];
};

using GamePadButton = unsigned int;


enum class BTN
{
	UP,
	RIGHT,
	DOWN,
	LEFT,
	A,
	B,
	X,
	Y,
	START,
	BACK,
	LS,
	RS,
	LB,
	RB,
	LT,
	RT,
	END,
};

//ジョイパッド専用
class InputControllerJoyPad
{
public:
	InputControllerJoyPad();
	~InputControllerJoyPad();

	void Update();

	int GetKeyState(const INPUT_KEY& key);

	// ボタン入力状態の取得
	GamePadButton GetButton() const {
		return buttonState[0];
	}

	// ボタン押下状態の取得
	int GetButtonDown(BTN button) const {
		return Buttonstate[static_cast<int>(button)];
	}

	// ボタン押上状態の取得
	GamePadButton GetButtonUp() const { return buttonUp; }

	// 左スティックX軸入力状態の取得
	float GetAxisLX() const {
		//入力していないのに小さい値が入っているのでその場合は0で返す
		if (fabsf(axisLx) - AxisTolerance < 0)return 0;
		return axisLx;
	}


	// 左スティックY軸入力状態の取得
	float GetAxisLY() const {		//入力していないのに小さい値が入っているのでその場合は0で返す
		if (fabsf(axisLy) - AxisTolerance < 0)return 0;
		return axisLy;
	}

	// 右スティックX軸入力状態の取得
	float GetAxisRX() const {
		//入力していないのに小さい値が入っているのでその場合は0で返す
		if (fabsf(axisRx) - AxisTolerance < 0)return 0;
		return axisRx;
	}

	// 右スティックY軸入力状態の取得
	float GetAxisRY() const {
		//入力していないのに小さい値が入っているのでその場合は0で返す
		if (fabsf(axisRy) - AxisTolerance < 0)return 0;
		return axisRy;
	}

	// 左トリガー入力状態の取得
	float GetTriggerL() const { return triggerL; }

	// 右トリガー入力状態の取得
	float GetTriggerR() const { return triggerR; }


private:
	
	void XInputUpdate();
	void JoyConUpdate();

	GamePadButton newButtonState = 0;
	GamePadButton		buttonState[2] = { 0 };
	GamePadButton		buttonDown = 0;
	GamePadButton		buttonUp = 0;
	float				axisLx = 0.0f;
	float				axisLy = 0.0f;
	float				axisRx = 0.0f;
	float				axisRy = 0.0f;
	float				triggerL = 0.0f;
	float				triggerR = 0.0f;
	int					slot = 0;

	static const GamePadButton BTN_UP = (1 << 0);
	static const GamePadButton BTN_RIGHT = (1 << 1);
	static const GamePadButton BTN_DOWN = (1 << 2);
	static const GamePadButton BTN_LEFT = (1 << 3);
	static const GamePadButton BTN_A = (1 << 4);
	static const GamePadButton BTN_B = (1 << 5);
	static const GamePadButton BTN_X = (1 << 6);
	static const GamePadButton BTN_Y = (1 << 7);
	static const GamePadButton BTN_START = (1 << 8);
	static const GamePadButton BTN_BACK = (1 << 9);
	static const GamePadButton BTN_LS = (1 << 10);
	static const GamePadButton BTN_RS = (1 << 11);
	static const GamePadButton BTN_LB = (1 << 12);
	static const GamePadButton BTN_RB = (1 << 13);
	static const GamePadButton BTN_LT = (1 << 14);
	static const GamePadButton BTN_RT = (1 << 15);
private:
	int Buttonstate[static_cast<int>(BTN::END)];
	const float AxisTolerance = 0.1f;

};

class InputController
{
public:
	InputController();
	~InputController();

	
	void Update(const HWND& hWnd);

private:

	void UpdataKeyboard();

private:
	short inputKeyList[KEY_MAX];

public:
	InputControllerMouse* inputControllerMouse = nullptr;
	InputControllerKeyBoard* inputControllerKeyBoard = nullptr;
	InputControllerJoyPad* inputControllerJoyPad = nullptr;
	
};