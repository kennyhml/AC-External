#include "esp.h"
#include "player/player.h"
#include <thread>
#include <sstream>
#include "settings.h"

DWORD WINAPI ESP::Run(HANDLE hProcess, uintptr_t modBaseAddress, uintptr_t localPlayerAddress, uintptr_t entityListAddress, bool& exit)
{
	TargetWnd = FindWindow(0, "AssaultCube");
	HDC_Desktop = GetDC(TargetWnd);
	SetupDrawing(HDC_Desktop, TargetWnd);

	COLORREF cfEnemy = RGB(255, 0, 0);
	COLORREF cfTeam = RGB(0, 255, 0);

	HBRUSH hBrushEnemy = CreateSolidBrush(cfEnemy);
	HBRUSH hBrushTeam = CreateSolidBrush(cfTeam);


	while (exit)
	{
		DrawCircle(640 / 2, 480 / 2, settings::aimbotRadius * 3, 1, cfTeam);
		if (!settings::esp) { continue; }

		GetWindowRect(FindWindow(NULL, "AssaultCube"), &rect);

		int playerCount = GetPlayerCount(hProcess, modBaseAddress);
		auto players = LoadPlayers(hProcess, playerCount, entityListAddress);
		Player localPlayer = LoadPlayer(hProcess, localPlayerAddress);
		Vector3 screen;
		float matrix[16];

		GameMode mode = GetGameMode(hProcess, modBaseAddress);

		ReadProcessMemory(hProcess, (BYTE*)0x501AE8, &matrix, sizeof(matrix), nullptr);
		for (Player& player : players)
		{
			if (player.status != Status::Alive) { continue; }

			// Get the position from the view matrix, false if the player isnt visible to us
			if (!WorldToScreen(player.feetPos, screen, matrix, 640, 480)) { continue; }

			HBRUSH hBrush = player.isEnemy(localPlayer.team, mode) ? hBrushEnemy : hBrushTeam;
			COLORREF cf = player.isEnemy(localPlayer.team, mode) ? cfEnemy : cfTeam;

			float distance = GetDistance(localPlayer.headPos, player.headPos);

			int rectW = 1100 / distance;
			int rectH = 2000 / distance;
			DrawBorderBox(screen.x - (rectW / 2), screen.y - rectH, rectW, rectH, 1, hBrush);
			DrawLine((rect.right - rect.left) / 2, rect.bottom - rect.top, screen.x, screen.y, cf);

			std::stringstream oss;
			oss << std::fixed << (int)distance << "m";

			const std::string resultString = oss.str();
			const char* cstr = resultString.c_str();

			std::stringstream hss;
			hss << std::fixed << player.health << " HP";

			const std::string hRes = hss.str();
			const char* hstr = hRes.c_str();

			DrawString(screen.x, screen.y, cf, player.name);
			DrawString(screen.x, screen.y + 15, cf, cstr);
			DrawString(screen.x, screen.y + 30, cf, hstr);
		}
	}
}

void ESP::DrawFilledRect(int x, int y, int w, int h, HBRUSH brushColor)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(HDC_Desktop, &rect, brushColor);
}

void ESP::DrawString(int x, int y, COLORREF color, const char* text)
{
	SetTextAlign(HDC_Desktop, TA_CENTER | TA_NOUPDATECP);
	SetBkColor(HDC_Desktop, RGB(0, 0, 0));
	SetBkMode(HDC_Desktop, TRANSPARENT);
	SetTextColor(HDC_Desktop, color);
	SelectObject(HDC_Desktop, Font);
	TextOutA(HDC_Desktop, x, y, text, strlen(text));
	DeleteObject(Font);

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

void ESP::DrawCircle(int x, int y, int radius, int thickness, COLORREF color)
{
	HPEN hOPen;
	HPEN hNPen = CreatePen(PS_SOLID, thickness, color);
	hOPen = (HPEN)SelectObject(HDC_Desktop, hNPen);
	Arc(HDC_Desktop, x - radius, y - radius, x + radius, y + radius, 0, 0, 0, 0);
	DeleteObject(SelectObject(HDC_Desktop, hOPen));
}

void ESP::SetupDrawing(HDC hDesktop, HWND handle)
{
	HDC_Desktop = hDesktop;
	Handle = handle;
}
