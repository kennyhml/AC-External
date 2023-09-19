#pragma once

#include <d3d9.h>

namespace gui
{
	// window boundaries
	constexpr int WIDTH = 550;
	constexpr int HEIGHT = 300;

	inline bool exit = true;

	// winapi window variables
	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = { };

	// window movement points
	inline POINTS position = {};

	// directX state variables
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = {};

	// window creation & destruction
	void CreateHWindow(const char* windowName, const char* className) noexcept;
	void DestroyHWindow() noexcept;

	// device creation & destruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	// ImGui creation & destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;


	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;

}