#include "esp.h"
#include "player/player.h"
#include <thread>
#include <sstream>
#include "settings.h"

/**
 * @brief Draws a line from one point to another with a given color.
 *
 * @param x1, y2 The starting point of the line.
 * @param x2, y2 The ending point of the line.
 * @param color The color to draw the line with.
 */
void ESP::DrawLine(int x1, int y1, int x2, int y2, COLORREF color)
{
	HPEN hNewPen = CreatePen(PS_SOLID, 2, color);
	HPEN hOldPen = (HPEN)SelectObject(HDC_Desktop, hNewPen);

	MoveToEx(HDC_Desktop, x1, y1, NULL);
	LineTo(HDC_Desktop, x2, y2);

	// Delete new pen and restore original pen
	DeleteObject(SelectObject(HDC_Desktop, hOldPen));
}

/**
 * @brief Draws a circle at a given position with a given radius.
 *
 * @param x, y The center of the circle.
 * @param radius The radius of the circle.
 * @param thickness The thickness of the circle line.
 * @param color The color to draw the circle with.
 */
void ESP::DrawCircle(int x, int y, int radius, int thickness, COLORREF color)
{
	HPEN hNewPen = CreatePen(PS_SOLID, thickness, color);
	HPEN hOldPen = (HPEN)SelectObject(HDC_Desktop, hNewPen);

	Arc(HDC_Desktop, x - radius, y - radius, x + radius, y + radius, 0, 0, 0, 0);
	DeleteObject(SelectObject(HDC_Desktop, hOldPen));
}

/**
 * @brief Draws a filled rectangle at the given region.
 *
 * @param x, y The top left corner of the rectangle.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param brushColor The color to draw the rectangle with.
 */
void ESP::DrawFilledRect(int x, int y, int w, int h, HBRUSH brushColor)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(HDC_Desktop, &rect, brushColor);
}

/**
 * @brief Draws text at the given location.
 *
 * @param x, y The position to draw the text at.
 * @param color The color to draw the text with.
 * @param text The text to draw.
 */
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

/**
 * @brief Draws a border box given a region.
 *
 * @param x, y The top left corner of the box.
 * @param w, h The width and height of the box.
 * @param thicknes The thickness of the box.
 * @param brushColor The color to draw the box with.
 */
void ESP::DrawBorderBox(int x, int y, int w, int h, int thickness, HBRUSH brushColor)
{
	DrawFilledRect(x, y, w, thickness, brushColor);
	DrawFilledRect(x, y, thickness, h, brushColor);
	DrawFilledRect((x + w), y, thickness, h, brushColor);
	DrawFilledRect(x, y + h, w + thickness, thickness, brushColor);
}

/**
 * @brief Draws the name, distance to and the health of the player on the screen.
 *
 * @param player The player to draw the information of.
 * @param distance The distance to the player (to display).
 * @param position The position of the player in 2D space.
 * @param color The color to draw the information with.
 */
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

/**
 * @brief Draws a player on the screen including a box around its position, a line to
 * its position from the player, the players name, health, armor and the distance to it.
 *
 * @param player The player to draw the information of.
 * @param player The local player.
 * @param matrix The games view matrix.
 * @param mode The GameMode of the current game.
 */
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

/**
 * @brief Entry point for the ESP thread, displays the ESP if enabled until the cheat
 * is terminated.
 *
 * @param hProcess The handle to the target process.
 * @param modBaseAddress The modules base address in memory.
 * @param localPlayerAddress The local player address in memory.
 * @param entityListAddress The entity list address in memory.
 * @param exit The exit flag of the GUI to shutdown the thread.
 */
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
