#pragma once

#include "stdafx.h"

namespace ESP
{
	inline HDC HDC_Desktop;
	inline HFONT Font;
	inline HWND TargetWnd;
	inline HWND Handle;
	inline COLORREF TextCOLOR;
	inline RECT rect;

	DWORD WINAPI Run(HANDLE hProcess, uintptr_t modBaseAddress, uintptr_t localPlayerAddress, uintptr_t entityListAddress, bool& exit);
	void DrawFilledRect(int x, int y, int w, int h, HBRUSH brushColor);
	void DrawBorderBox(int x, int y, int w, int h, int thickness, HBRUSH brushColor);
	void DrawLine(float StartX, float StartY, float EndX, float EndY, COLORREF Pen);
	void SetupDrawing(HDC hDesktop, HWND handle);
	void DrawString(int x, int y, COLORREF color, const char* text);
}
