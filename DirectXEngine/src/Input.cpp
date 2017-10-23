#include "Input.h"

using namespace DirectX;

Input* Input::m_instance = nullptr;

Input::Input(HWND window)
{
	if (!m_instance) m_instance = this;
	else return;

	m_keyboard = new Keyboard();
	m_keyboardState = m_keyboard->GetState();

	m_mouse = new Mouse();
	m_mouse->SetWindow(window);
	m_mouseState = m_mouse->GetState();

	m_gamePad = new GamePad();
	for (unsigned int i = 0; i < GamePad::MAX_PLAYER_COUNT; i++)
	{
		m_gamePadStates[0] = m_gamePad->GetState(0);
	}
}

Input::~Input()
{
	delete m_keyboard;
	delete m_mouse;

	m_gamePad->Suspend();
	delete m_gamePad;

	m_instance = nullptr;
}

void Input::update()
{
	m_keyboardTracker.Update(m_keyboardState);
	m_keyboardState = m_keyboard->GetState();
	
	m_mouseTracker.Update(m_mouseState);
	m_mouseState = m_mouse->GetState();
	

	for (unsigned int i = 0; i < GamePad::MAX_PLAYER_COUNT; i++)
	{
		m_gamePadTracker[i].Update(m_gamePadStates[i]);
		m_gamePadStates[i] = m_gamePad->GetState(i);
	}
}

bool Input::isKeyPressed(DirectX::Keyboard::Keys key)
{
	return m_instance->m_keyboardTracker.IsKeyPressed(key);
}

bool Input::isKeyReleased(DirectX::Keyboard::Keys key)
{
	return m_instance->m_keyboardTracker.IsKeyReleased(key);
}

bool Input::isKeyDown(DirectX::Keyboard::Keys key)
{
	if (m_instance->m_keyboardState.IsKeyDown(key))
		return true;

	return false;
}

bool Input::isKeyUp(DirectX::Keyboard::Keys key)
{
	if (m_instance->m_keyboardState.IsKeyUp(key))
		return true;

	return false;
}

bool Input::isMouseButtonPressed(MouseButton button)
{
	switch (button)
	{
	case MOUSE_LEFT:
		return m_instance->m_mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED;

	case MOUSE_MIDDLE:
		return m_instance->m_mouseTracker.middleButton == Mouse::ButtonStateTracker::PRESSED;

	case MOUSE_RIGHT:
		return m_instance->m_mouseTracker.rightButton == Mouse::ButtonStateTracker::PRESSED;

	default:
		return false;
	}
}

bool Input::isMouseButtonReleased(MouseButton button)
{
	switch (button)
	{
	case MOUSE_LEFT:
		return m_instance->m_mouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED;

	case MOUSE_MIDDLE:
		return m_instance->m_mouseTracker.middleButton == Mouse::ButtonStateTracker::RELEASED;

	case MOUSE_RIGHT:
		return m_instance->m_mouseTracker.rightButton == Mouse::ButtonStateTracker::RELEASED;

	default:
		return false;
	}
}

bool Input::isMouseButtonDown(MouseButton button)
{
	switch (button)
	{
	case MOUSE_LEFT:
		return m_instance->m_mouseState.leftButton;

	case MOUSE_MIDDLE:
		return m_instance->m_mouseState.middleButton;

	case MOUSE_RIGHT:
		return m_instance->m_mouseState.rightButton;

	default:
		return false;
	}
	
}

bool Input::isMouseButtonUp(MouseButton button)
{
	switch (button)
	{
	case MOUSE_LEFT:
		return !m_instance->m_mouseState.leftButton;

	case MOUSE_MIDDLE:
		return !m_instance->m_mouseState.middleButton;

	case MOUSE_RIGHT:
		return !m_instance->m_mouseState.rightButton;

	default:
		return false;
	}
}

DirectX::XMFLOAT2 Input::getMousePositon()
{
	return XMFLOAT2((float)m_instance->m_mouseState.x, (float)m_instance->m_mouseState.y);
}

DirectX::XMFLOAT2 Input::getMouseDelta()
{
	Mouse::State prevMouseState = m_instance->m_mouseTracker.GetLastState();
	return XMFLOAT2((float)m_instance->m_mouseState.x - (float)prevMouseState.x, (float)m_instance->m_mouseState.y - (float)prevMouseState.y);
}

bool Input::isGamePadButtonPressed(int gamepadID, GamePadButtons button)
{
	if (!m_instance->m_gamePadStates[gamepadID].IsConnected()) return false;

	switch (button)
	{
	case A:
		return m_instance->m_gamePadTracker[gamepadID].a == GamePad::ButtonStateTracker::PRESSED;

	case B:
		return m_instance->m_gamePadTracker[gamepadID].b == GamePad::ButtonStateTracker::PRESSED;

	case X:
		return m_instance->m_gamePadTracker[gamepadID].x == GamePad::ButtonStateTracker::PRESSED;

	case Y:
		return m_instance->m_gamePadTracker[gamepadID].y == GamePad::ButtonStateTracker::PRESSED;

	case LEFT_SHOULDER:
		return m_instance->m_gamePadTracker[gamepadID].leftShoulder == GamePad::ButtonStateTracker::PRESSED;

	case RIGHT_SHOULDER:
		return m_instance->m_gamePadTracker[gamepadID].rightShoulder == GamePad::ButtonStateTracker::PRESSED;

	case LEFT_STICK:
		return m_instance->m_gamePadTracker[gamepadID].leftStick == GamePad::ButtonStateTracker::PRESSED;

	case RIGHT_STICK:
		return m_instance->m_gamePadTracker[gamepadID].rightStick == GamePad::ButtonStateTracker::PRESSED;

	case DPAD_LEFT:
		return m_instance->m_gamePadTracker[gamepadID].dpadLeft == GamePad::ButtonStateTracker::PRESSED;

	case DPAD_RIGHT:
		return m_instance->m_gamePadTracker[gamepadID].dpadRight == GamePad::ButtonStateTracker::PRESSED;

	case DPAD_UP:
		return m_instance->m_gamePadTracker[gamepadID].dpadUp == GamePad::ButtonStateTracker::PRESSED;

	case DPAD_DOWN:
		return m_instance->m_gamePadTracker[gamepadID].dpadDown == GamePad::ButtonStateTracker::PRESSED;

	case START:
		return m_instance->m_gamePadTracker[gamepadID].start == GamePad::ButtonStateTracker::PRESSED;

	case BACK:
		return m_instance->m_gamePadTracker[gamepadID].back == GamePad::ButtonStateTracker::PRESSED;

	default:
		return false;
	}
}

bool Input::isGamePadButtonReleased(int gamepadID, GamePadButtons button)
{
	if (!m_instance->m_gamePadStates[gamepadID].IsConnected()) return false;
	
	switch (button)
	{
	case A:
		return m_instance->m_gamePadTracker[gamepadID].a == GamePad::ButtonStateTracker::RELEASED;

	case B:
		return m_instance->m_gamePadTracker[gamepadID].b == GamePad::ButtonStateTracker::RELEASED;

	case X:
		return m_instance->m_gamePadTracker[gamepadID].x == GamePad::ButtonStateTracker::RELEASED;

	case Y:
		return m_instance->m_gamePadTracker[gamepadID].y == GamePad::ButtonStateTracker::RELEASED;

	case LEFT_SHOULDER:
		return m_instance->m_gamePadTracker[gamepadID].leftShoulder == GamePad::ButtonStateTracker::RELEASED;

	case RIGHT_SHOULDER:
		return m_instance->m_gamePadTracker[gamepadID].rightShoulder == GamePad::ButtonStateTracker::RELEASED;

	case LEFT_STICK:
		return m_instance->m_gamePadTracker[gamepadID].leftStick == GamePad::ButtonStateTracker::RELEASED;

	case RIGHT_STICK:
		return m_instance->m_gamePadTracker[gamepadID].rightStick == GamePad::ButtonStateTracker::RELEASED;

	case DPAD_LEFT:
		return m_instance->m_gamePadTracker[gamepadID].dpadLeft == GamePad::ButtonStateTracker::RELEASED;

	case DPAD_RIGHT:
		return m_instance->m_gamePadTracker[gamepadID].dpadRight == GamePad::ButtonStateTracker::RELEASED;

	case DPAD_UP:
		return m_instance->m_gamePadTracker[gamepadID].dpadUp == GamePad::ButtonStateTracker::RELEASED;

	case DPAD_DOWN:
		return m_instance->m_gamePadTracker[gamepadID].dpadDown == GamePad::ButtonStateTracker::RELEASED;

	case START:
		return m_instance->m_gamePadTracker[gamepadID].start == GamePad::ButtonStateTracker::RELEASED;

	case BACK:
		return m_instance->m_gamePadTracker[gamepadID].back == GamePad::ButtonStateTracker::RELEASED;

	default:
		return false;
	}
}

bool Input::isGamePadButtonDown(int gamepadID, GamePadButtons button)
{
	if (!m_instance->m_gamePadStates[gamepadID].IsConnected()) return false;
	
	switch (button)
	{
	case A:
		return m_instance->m_gamePadStates[gamepadID].IsAPressed();

	case B:
		return m_instance->m_gamePadStates[gamepadID].IsBPressed();

	case X:
		return m_instance->m_gamePadStates[gamepadID].IsXPressed();

	case Y:
		return m_instance->m_gamePadStates[gamepadID].IsYPressed();

	case LEFT_SHOULDER:
		return m_instance->m_gamePadStates[gamepadID].IsLeftShoulderPressed();

	case RIGHT_SHOULDER:
		return m_instance->m_gamePadStates[gamepadID].IsRightShoulderPressed();

	case LEFT_STICK:
		return m_instance->m_gamePadStates[gamepadID].IsLeftStickPressed();

	case RIGHT_STICK:
		return m_instance->m_gamePadStates[gamepadID].IsRightStickPressed();

	case DPAD_LEFT:
		return m_instance->m_gamePadStates[gamepadID].IsDPadLeftPressed();

	case DPAD_RIGHT:
		return m_instance->m_gamePadStates[gamepadID].IsDPadRightPressed();

	case DPAD_UP:
		return m_instance->m_gamePadStates[gamepadID].IsDPadUpPressed();

	case DPAD_DOWN:
		return m_instance->m_gamePadStates[gamepadID].IsDPadDownPressed();

	case START:
		return m_instance->m_gamePadStates[gamepadID].IsStartPressed();

	case BACK:
		return m_instance->m_gamePadStates[gamepadID].IsBackPressed();

	default:
		return false;
	}
}

bool Input::isGamePadButtonUp(int gamepadID, GamePadButtons button)
{
	if (!m_instance->m_gamePadStates[gamepadID].IsConnected()) return false;
	
	switch (button)
	{
	case A:
		return !m_instance->m_gamePadStates[gamepadID].IsAPressed();

	case B:
		return !m_instance->m_gamePadStates[gamepadID].IsBPressed();

	case X:
		return !m_instance->m_gamePadStates[gamepadID].IsXPressed();

	case Y:
		return !m_instance->m_gamePadStates[gamepadID].IsYPressed();

	case LEFT_SHOULDER:
		return !m_instance->m_gamePadStates[gamepadID].IsLeftShoulderPressed();

	case RIGHT_SHOULDER:
		return !m_instance->m_gamePadStates[gamepadID].IsRightShoulderPressed();

	case LEFT_STICK:
		return !m_instance->m_gamePadStates[gamepadID].IsLeftStickPressed();

	case RIGHT_STICK:
		return !m_instance->m_gamePadStates[gamepadID].IsRightStickPressed();

	case DPAD_LEFT:
		return !m_instance->m_gamePadStates[gamepadID].IsDPadLeftPressed();

	case DPAD_RIGHT:
		return !m_instance->m_gamePadStates[gamepadID].IsDPadRightPressed();

	case DPAD_UP:
		return !m_instance->m_gamePadStates[gamepadID].IsDPadUpPressed();

	case DPAD_DOWN:
		return !m_instance->m_gamePadStates[gamepadID].IsDPadDownPressed();

	case START:
		return !m_instance->m_gamePadStates[gamepadID].IsStartPressed();

	case BACK:
		return !m_instance->m_gamePadStates[gamepadID].IsBackPressed();

	default:
		return false;
	}
}

float Input::getGamePadTrigger(int gamepadID, GamePadTriggers trigger)
{
	if (!m_instance->m_gamePadStates[gamepadID].IsConnected()) return 0.0f;
	
	switch (trigger)
	{
	case TRIGGER_LEFT:
		return m_instance->m_gamePadStates[gamepadID].triggers.left;

	case TRIGGER_RIGHT:
		return m_instance->m_gamePadStates[gamepadID].triggers.right;

	default:
		return 0.0f;
	}
}

float Input::getGamePadStick(int gamepadID, GamePadThumbsticks stick, ThumbstickAxis axis)
{
	if (!m_instance->m_gamePadStates[gamepadID].IsConnected()) return 0.0f;
	
	switch (stick)
	{
	case THUMBSTICK_LEFT:
		switch (axis)
		{
		case XAXIS:
			return m_instance->m_gamePadStates[gamepadID].thumbSticks.leftX;

		case YAXIS:
			return m_instance->m_gamePadStates[gamepadID].thumbSticks.leftY;

		default:
			return 0.0f;
		}

	case THUMBSTICK_RIGHT:
		switch (axis)
		{
		case XAXIS:
			return m_instance->m_gamePadStates[gamepadID].thumbSticks.rightX;

		case YAXIS:
			return m_instance->m_gamePadStates[gamepadID].thumbSticks.rightY;

		default:
			return 0.0f;
		}

	default:
		return 0.0f;
	}
}

void Input::resume()
{
	if (m_instance)
	{
		m_instance->m_gamePad->Resume();

		m_instance->m_keyboardTracker.Reset();
		m_instance->m_mouseTracker.Reset();
		m_instance->m_gamePadTracker->Reset();
	}
}

void Input::suspend()
{
	if (m_instance)
	{
		m_instance->m_gamePad->Suspend();
	}
}

void Input::ProcessKeyboardMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	Keyboard::ProcessMessage(msg, wParam, lParam);
}

void Input::ProcessMouseMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	Mouse::ProcessMessage(msg, wParam, lParam);
}
