#pragma once

#include "stdafx.h"

namespace ESP
{
	inline HDC HDC_Desktop;
	inline HFONT font;
	inline HWND TargetWnd;
	inline HWND Handle;
	inline RECT rect;

	inline COLORREF cfEnemy;
	inline COLORREF cfTeam;

	inline HBRUSH hBrushEnemy;
	inline HBRUSH hBrushTeam;

	DWORD WINAPI Run(HANDLE hProcess, uintptr_t modBaseAddress, uintptr_t localPlayerAddress, uintptr_t entityListAddress, bool& exit);
	void DrawFilledRect(int x, int y, int w, int h, HBRUSH brushColor);
	void DrawBorderBox(int x, int y, int w, int h, int thickness, HBRUSH brushColor);
	void DrawLine(int x1, int y1, int x2, int y2, COLORREF color);
	void DrawCircle(int x, int y, int radius, int thickness, COLORREF color);
	void SetupDrawing(HDC hDesktop, HWND handle);
	void DrawString(int x, int y, COLORREF color, const char* text);

}
