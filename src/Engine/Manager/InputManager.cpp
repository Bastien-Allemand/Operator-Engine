#include "pch.h"
#include "InputManager.h"
#include "Core/Window.h"

InputManager* InputManager::m_instance = nullptr;

InputManager::InputManager()
{
    DebugMsg(L"InputManager Singleton Created", DebugFlag::LOG);
}

InputManager* InputManager::GInstance()
{
    if (m_instance == nullptr)
    {
        m_instance = new InputManager();
    }
    return m_instance;
}

void InputManager::Init(Window* _window)
{
	m_initialized = true;
	m_window = _window;
}

void InputManager::Update()
{
    if (!m_initialized) return;
	memcpy(m_prevKeyState, m_keyState, sizeof(m_keyState));
    if (!m_window->IsFocused())
    {
		memset(m_keyState, 0, sizeof(m_keyState));
        return;
    }

    for (int i = 0; i < 256; i++)
    {
		m_keyState[i] = GetAsyncKeyState(i) < 0;
    }
}

bool InputManager::IsKeyDown(int32 _vKey)
{
    if (_vKey < 0 || _vKey >= 256) return false;
	return m_keyState[_vKey];
}

bool InputManager::IsKeyPressed(int32 _vKey)
{
    bool isDown = IsKeyDown(_vKey);
	return isDown && !m_prevKeyState[_vKey];
}

bool InputManager::IsKeyReleased(int32 _vKey)
{
    bool isDown = IsKeyDown(_vKey);
    return !isDown && m_prevKeyState[_vKey];
}