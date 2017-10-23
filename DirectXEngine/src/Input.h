#pragma once

#include <Windows.h>

#include <Keyboard.h>
#include <Mouse.h>
#include <GamePad.h>

#include <DirectXMath.h>

#define MAX_GAMEPADS 4

enum MouseButton
{
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT
};

enum GamePadButtons
{
	A,
	B,
	X,
	Y,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	LEFT_STICK,
	RIGHT_STICK,
	DPAD_LEFT,
	DPAD_RIGHT,
	DPAD_UP,
	DPAD_DOWN,
	START,
	BACK
};

enum GamePadTriggers
{
	TRIGGER_LEFT,
	TRIGGER_RIGHT
};

enum GamePadThumbsticks
{
	THUMBSTICK_LEFT,
	THUMBSTICK_RIGHT
};

enum ThumbstickAxis
{
	XAXIS,
	YAXIS
};

class Input
{
public:
	Input(HWND window);
	~Input();

	void update();

	static bool isKeyPressed(DirectX::Keyboard::Keys key);
	static bool isKeyReleased(DirectX::Keyboard::Keys key);

	static bool isKeyDown(DirectX::Keyboard::Keys key);
	static bool isKeyUp(DirectX::Keyboard::Keys key);

	static bool isMouseButtonPressed(MouseButton button);
	static bool isMouseButtonReleased(MouseButton button);

	static bool isMouseButtonDown(MouseButton button);
	static bool isMouseButtonUp(MouseButton button);

	static DirectX::XMFLOAT2 getMousePositon();
	static DirectX::XMFLOAT2 getMouseDelta();

	static bool isGamePadButtonPressed(int gamepadID, GamePadButtons button);
	static bool isGamePadButtonReleased(int gamepadID, GamePadButtons button);

	static bool isGamePadButtonDown(int gamepadID, GamePadButtons button);
	static bool isGamePadButtonUp(int gamepadID, GamePadButtons button);

	static float getGamePadTrigger(int gamepadID, GamePadTriggers trigger);
	static float getGamePadStick(int gamepadID, GamePadThumbsticks stick, ThumbstickAxis axis);

	static void resume();
	static void suspend();
	
	static void ProcessKeyboardMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	static void ProcessMouseMessage(UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static Input* m_instance;

	DirectX::Keyboard* m_keyboard;
	DirectX::Mouse* m_mouse;
	DirectX::GamePad* m_gamePad;

	DirectX::Keyboard::State m_keyboardState;
	DirectX::Keyboard::KeyboardStateTracker m_keyboardTracker;

	DirectX::Mouse::State m_mouseState;
	DirectX::Mouse::ButtonStateTracker m_mouseTracker;

	DirectX::GamePad::State m_gamePadStates[DirectX::GamePad::MAX_PLAYER_COUNT];
	DirectX::GamePad::ButtonStateTracker m_gamePadTracker[DirectX::GamePad::MAX_PLAYER_COUNT];

};
