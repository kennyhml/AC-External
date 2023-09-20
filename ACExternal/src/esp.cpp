#include "esp.h"
#include "player/player.h"
#include <thread>
#include <sstream>
#include "settings.h"


void ESP::DrawLine(int x1, int y1, int x2, int y2, COLORREF color)
{
	HPEN hNewPen = CreatePen(PS_SOLID, 2, color);
	HPEN hOldPen = (HPEN)SelectObject(HDC_Desktop, hNewPen);

	MoveToEx(HDC_Desktop, x1, y1, NULL);
	LineTo(HDC_Desktop, x2, y2);

	// Delete new pen and restore original pen
	DeleteObject(SelectObject(HDC_Desktop, hOldPen));
}

void ESP::DrawCircle(int x, int y, int radius, int thickness, COLORREF color)
{
	HPEN hNewPen = CreatePen(PS_SOLID, thickness, color);
	HPEN hOldPen = (HPEN)SelectObject(HDC_Desktop, hNewPen);

	Arc(HDC_Desktop, x - radius, y - radius, x + radius, y + radius, 0, 0, 0, 0);
	DeleteObject(SelectObject(HDC_Desktop, hOldPen));
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
	SelectObject(HDC_Desktop, font);
	TextOutA(HDC_Desktop, x, y, text, strlen(text));
	DeleteObject(font);

}

void ESP::DrawBorderBox(int x, int y, int w, int h, int thickness, HBRUSH brushColor)
{
	DrawFilledRect(x, y, w, thickness, brushColor);
	DrawFilledRect(x, y, thickness, h, brushColor);
	DrawFilledRect((x + w), y, thickness, h, brushColor);
	DrawFilledRect(x, y + h, w + thickness, thickness, brushColor);
}

static void DrawPlayerData(const Player& player, int distance, Vector2 position, COLORREF color)
{
	std::stringstream distanceStream;
	distanceStream << " (" << (int)distance << "m)";
	std::string name = std::string(player.name) + distanceStream.str();
	const char* nameInfo = name.c_str();

	std::stringstream healthStream;
	healthStream << player.health + player.armor << "/200";

	const std::string health = healthStream.str();
	const char* healthInfo = health.c_str();

	ESP::DrawString(position.x, position.y, color, nameInfo);
	ESP::DrawString(position.x, position.y + 15, color, healthInfo);
}

static void DrawPlayer(const Player& player, const Player& localPlayer, float matrix[16], GameMode mode)
{
	// Don't render dead players
	if (player.status != Status::Alive) { return; }

	Vector2 position;
	// Don't render players that are not within our field of view (behind, above...)
	if (!WorldToScreen(player.feetPos, position, matrix, 640, 480)) { return; }

	// Get the appropriate color, green for team and red for enemies.
	bool isPlayerEnemy = player.isEnemy(localPlayer.team, mode);
	HBRUSH hBrushColor = isPlayerEnemy ? ESP::hBrushEnemy : ESP::hBrushTeam;
	COLORREF refColor = isPlayerEnemy ? ESP::cfEnemy : ESP::cfTeam;

	// Calculate the distance to scale the size of the bounding rect
	int distance = (int)GetDistance(localPlayer.headPos, player.headPos);
	int rectWidth = 1100 / distance;
	int rectHeight = 2000 / distance;

	// calculate the topleft position of our bounding rectangle
	int x = position.x - (rectWidth / 2);
	int y = position.y - rectHeight;

	// Calculate the beginning of the line to the player
	int center = (int)(ESP::rect.right - ESP::rect.left) / 2;
	int bottom = (int)ESP::rect.bottom - ESP::rect.top;

	ESP::DrawBorderBox(x, y, rectWidth, rectHeight, 1, hBrushColor);
	ESP::DrawLine(center, bottom, position.x, position.y, refColor);
	DrawPlayerData(player, distance, position, refColor);
}

DWORD WINAPI ESP::Run(HANDLE hProcess, uintptr_t modBaseAddress, uintptr_t localPlayerAddress, uintptr_t entityListAddress, bool& exit)
{
	TargetWnd = FindWindow(0, "AssaultCube");
	HDC_Desktop = GetDC(TargetWnd);
	SetupDrawing(HDC_Desktop, TargetWnd);

	cfEnemy = RGB(255, 0, 0);
	cfTeam = RGB(0, 255, 0);
	hBrushEnemy = CreateSolidBrush(cfEnemy);
	hBrushTeam = CreateSolidBrush(cfTeam);

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
		for (const Player& player : players)
		{
			DrawPlayer(player, localPlayer, matrix, mode);
		}
	}
}

void ESP::SetupDrawing(HDC hDesktop, HWND handle)
{
	HDC_Desktop = hDesktop;
	Handle = handle;
}
