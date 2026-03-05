#pragma once
class Window;

class InputManager
{
public:
    static InputManager* GInstance();

    void Init(Window* _window);

    void Update();

	void ResetKeyStates() { memset(m_keyState, 0, sizeof(m_keyState)); memset(m_prevKeyState, 0, sizeof(m_prevKeyState)); }
    /// <summary>
	/// is key pressed (held down) this frame
    /// </summary>
    bool IsKeyDown(int32 _vKey);    
    /// <summary>
	/// is key just pressed this frame (transition from up to down)
    /// </summary>
    bool IsKeyPressed(int32 _vKey);  
    /// <summary>
	/// is key just released this frame (transition from down to up)
    /// </summary>
    bool IsKeyReleased(int32 _vKey);  
	/// <summary>
	/// is key up (not pressed) this frame
	/// </summary>
	bool IskeyUp(int32 _vKey) { return !IsKeyDown(_vKey); }
private:
    InputManager();
    ~InputManager() = default;
	bool m_initialized = false;

    static InputManager* m_instance;

	Window* m_window = nullptr;
    
	bool m_keyState[256] = { false };
	bool m_prevKeyState[256] = { false };
};