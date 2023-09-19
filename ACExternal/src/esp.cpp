#include "esp.h"
#include "player/player.h"
#include <thread>



DWORD WINAPI ESP::Run(HANDLE hProcess, uintptr_t modBaseAddress, uintptr_t localPlayerAddress, uintptr_t entityListAddress, bool& exit)
{
	while (exit)
	{
		GetWindowRect(FindWindow(NULL, "AssaultCube"), &rect);

		int playerCount = GetPlayerCount(hProcess, modBaseAddress);
		auto players = LoadPlayers(hProcess, playerCount, entityListAddress);
		Player localPlayer = LoadPlayer(hProcess, localPlayerAddress);
		Vector3 screen;
		float matrix[16];

		ReadProcessMemory(hProcess, (BYTE*)0x501AE8, &matrix, sizeof(matrix), nullptr);
		for (Player& enemy : players)
		{
			if (WorldToScreen(enemy.feetPos, screen, matrix, 640, 480))
			{
				float distance = GetDistance(localPlayer.headPos, enemy.headPos);
				int rectW = 1100 / distance;
				int rectH = 2000 / distance;

				DrawBorderBox(screen.x - (rectW / 2), screen.y - rectH, rectW, rectH, 1, CreateSolidBrush(RGB(255, 0, 0)));
			}
		}
	}
}

void ESP::DrawFilledRect(int x, int y, int w, int h, HBRUSH brushColor)
{
	RECT rect = { x,y,x + w,y + h };
	FillRect(HDC_Desktop, &rect, brushColor);
}

void ESP::DrawBorderBox(int x, int y, int w, int h, int thickness, HBRUSH brushColor)
{
	DrawFilledRect(x, y, w, thickness, brushColor);
	DrawFilledRect(x, y, thickness, h, brushColor);
	DrawFilledRect((x + w), y, thickness, h, brushColor);
	DrawFilledRect(x, y + h, w + thickness, thickness, brushColor);
}

void ESP::DrawLine(float StartX, float StartY, float EndX, float EndY, COLORREF Pen)
{
	int a, b = 0;
	HPEN hOPen;
	HPEN hNPen = CreatePen(PS_SOLID, 2, Pen);
	hOPen = (HPEN)SelectObject(HDC_Desktop, hNPen);
	MoveToEx(HDC_Desktop, StartX, StartY, NULL);
	a = LineTo(HDC_Desktop, EndX, EndY);
	DeleteObject(SelectObject(HDC_Desktop, hOPen));
}

void ESP::SetupDrawing(HDC hDesktop, HWND handle)
{
	HDC_Desktop = hDesktop;
	Handle = handle;
	TextCOLOR = RGB(0, 255, 0);
}
