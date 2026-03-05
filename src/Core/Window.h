#pragma once

class Window
{
public:
	Window() = default;
	bool Init(int _width, int _height, const wchar_t* _title);
	int GCWidth();
	int GCHeight();
	bool IsFocused() const;
	void HideCursor(bool _hide);
	HWND GHWND() const { return m_hwnd; }
private:
	static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);
	HWND m_hwnd = nullptr;
	int m_width = -1;
	int m_height = -1;
};

