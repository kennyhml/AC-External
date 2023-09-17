#include "gui.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include "settings.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{

	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter)) { return true; };

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;
		
	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) { return 0; };
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	} return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter);
	}return 0;

	case WM_MOUSEMOVE: {

		if (wideParameter != MK_LBUTTON) { break; };

		const auto points = MAKEPOINTS(longParameter);
		auto rect = ::RECT{};

		GetWindowRect(gui::window, &rect);
		rect.left += points.x - gui::position.x;
		rect.top += points.y - gui::position.y;

		if (gui::position.x >= 0 &&
			gui::position.x <= gui::WIDTH &&
			gui::position.y >= 0 &&
			gui::position.y <= 19)
		{
			SetWindowPos(
				gui::window,
				HWND_TOPMOST,
				rect.left, rect.top,
				0, 0,
				SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
		}
	}return 0;

	}

	return DefWindowProcW(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName, const char* className) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);
	window = CreateWindowA(
		className,
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);
	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) { return false; };

	ZeroMemory(&presentParameters, sizeof(presentParameters));
	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	return (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) >= 0);
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL) { IM_ASSERT(0); };
	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device) 
	{
		device->Release();
		device = nullptr;
	}

	if (d3d) 
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;

	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ResetDevice();
	}



}

void gui::Render() noexcept
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"AC-External: For crosseyed gamers.",
		&exit,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);

	// Create a table with 4 columns
	if (ImGui::BeginTable("CheckboxTable", 4, ImGuiTableFlags_NoBordersInBody))
	{
		// Checkbox labels and values
		const char* checkboxLabels[] = {
			"Fly Hack", "Ghost Mode", "God Mode", "No Gravity",
			"No Recoil", "No Spray", "No Reload", "Rapid Fire", 
			"Inf. Ammo", "No Shake", "Full Auto", "One Tap",
			"Aimbot"
		};
		bool* checkboxValues[] = {
			&settings::flyHack, &settings::ghostMode, &settings::godMode, &settings::antiGravity,
			&settings::noRecoil, &settings::noSpray, &settings::instantReload, &settings::rapidFire, 
			&settings::infiniteAmmo, &settings::noShake, &settings::fullAuto, &settings::oneTap,
			&settings::aimbot
		};

		for (int i = 0; i < 13; i++)
		{
			if (i % 4 == 0)
			{
				// Start a new row for every 4 checkboxes
				ImGui::TableNextRow();
			}

			// Create the checkbox
			ImGui::TableSetColumnIndex(i % 4); // Set the column index modulo 4
			ImGui::Checkbox(checkboxLabels[i], checkboxValues[i]);
		}

		ImGui::EndTable();
	}

	ImGui::NewLine();
	// Slider
	ImGui::SetNextItemWidth(90.0f);
	ImGui::SliderInt("Speed", &settings::speed, 1, 10);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(90.0f);
	ImGui::SliderInt("Knockback", &settings::knockback, -100, 100);

	static int selectedItem = 0;
	const char* items[] = { "Assault Rifle", "Sniper Rifle", "Shotgun", "Grenade", "Carbine", "Akimbo"};
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	if (ImGui::Combo("Weapon", &selectedItem, items, IM_ARRAYSIZE(items))) 
	{
		settings::selectedWeapon = items[selectedItem];
	}
	else
	{
		settings::selectedWeapon = NULL;
	}

	ImGui::End();
}