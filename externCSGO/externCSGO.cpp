#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <thread>
#include "glow.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "chams.h"

#include <dwmapi.h>
#include <d3dx9.h>
#include "imgui.h"

#include "player.h"
#include "Aimbot.h"
#include "glow.h"
#include "Overlay.h"
#include "box.h"


LPCSTR TargetProcess = "csgo.exe";
bool ShowMenu = true;
bool CreateConsole = true;

struct CurrentProcess {
	DWORD ID;
	HANDLE Handle;
	HWND Hwnd;
	WNDPROC WndProc;
	int WindowWidth;
	int WindowHeight;
	int WindowLeft;
	int WindowRight;
	int WindowTop;
	int WindowBottom;
	LPCSTR Title;
	LPCSTR ClassName;
	LPCSTR Path;
}Process;

struct OverlayWindow {
	WNDCLASSEX WindowClass;
	HWND Hwnd;
	LPCSTR Name;
}Overlay;

struct DirectX9Interface {
	IDirect3D9Ex* IDirect3D9 = NULL;
	IDirect3DDevice9Ex* pDevice = NULL;
	D3DPRESENT_PARAMETERS pParameters = { NULL };
	MARGINS Margin = { -1 };
	MSG Message = { NULL };
}DirectX9;

void InputHandler() {
	for (int i = 0; i < 5; i++) {
		ImGui::GetIO().MouseDown[i] = false;
	}

	int Button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) {
		Button = 0;
	}

	if (Button != -1) {
		ImGui::GetIO().MouseDown[Button] = true;
	}
}

DWORD GetProcessId(LPCSTR ProcessName) {
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);
	return 0;
}

std::string RandomString(int len) {
	srand(time(NULL));
	std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string newstr;
	int pos;
	while (newstr.size() != len) {
		pos = ((rand() % (str.size() - 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}
void CenteredText(std::string text)
{
	float win_width = ImGui::GetWindowSize().x;
	float text_width = ImGui::CalcTextSize(text.c_str()).x;

	// calculate the indentation that centers the text on one line, relative
	// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
	float text_indentation = (win_width - text_width) * 0.5f;

	// if text is too long to be drawn on one line, `text_indentation` can
	// become too small or even negative, so we check a minimum indentation
	float min_indentation = 20.0f;
	if (text_indentation <= min_indentation) {
		text_indentation = min_indentation;
	}

	ImGui::SameLine(text_indentation);
	ImGui::PushTextWrapPos(win_width - text_indentation);
	ImGui::TextWrapped(text.c_str());
	ImGui::PopTextWrapPos();
}

void styles()
{

	ImGuiStyle* style = &ImGui::GetStyle();

	ImVec4* colors = style->Colors;



	style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.0f, 0.0263f, 0.0357f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImColor(25, 25, 25);
	style->Colors[ImGuiCol_ChildBg] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.080f, 0.080f, 0.080f, 0.940f);
	style->Colors[ImGuiCol_Border] = ImColor(51, 2, 2);
	style->Colors[ImGuiCol_BorderShadow] = ImColor(1, 1, 1);
	style->Colors[ImGuiCol_FrameBg] = ImColor(40, 40, 40);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(40, 40, 40);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(40, 40, 40);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.102f, 0.090f, 0.122f, 1.000f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.102f, 0.090f, 0.122f, 1.000f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.102f, 0.090f, 0.122f, 1.000f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.263f, 0.357f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImColor(255, 42, 0); // 
	style->Colors[ImGuiCol_SliderGrab] = ImColor(255, 42, 0); // 
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 42, 0); // 
	style->Colors[ImGuiCol_Button] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(20, 20, 20);
	style->Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	style->Colors[ImGuiCol_Separator] = ImColor(105, 0, 255);

}



void AlignForWidth(float width, float alignment = 0.5f)
{
	ImGuiStyle& style = ImGui::GetStyle();
	float avail = ImGui::GetContentRegionAvail().x;
	float off = (avail - width) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}



void draw()
{
	RGBA Colour = { 255,0,0,255 };
	DrawStrokeText(30, 22, &Colour, "Possible's Hackeroonies");
}

void BeginMenu()
{

	static int tab = 0;
	styles(); // start styliiiinnn


	ImGui::SetNextWindowSize(ImVec2(450.000f, 550.000f), ImGuiCond_Once);
	ImGui::Begin(("Possible's Hacks"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);



	ImGui::BeginChild(("##backround"), ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);
	{
		if (ImGui::Button("Aim", ImVec2(77, 20)))
		{
			tab = 1;
		}


		ImGui::SameLine();


		if (ImGui::Button("Visuals", ImVec2(77, 20)))
		{
			tab = 2;
		}

		ImGui::SameLine();

		if (ImGui::Button("Misc", ImVec2(77, 20)))
		{
			tab = 3;
		}

		if (tab == 1)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(255, 42, 0)));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(255, 42, 0)));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(255, 42, 0)));


			ImGui::Button(" ", ImVec2(77, 3));


			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(30, 30, 30, 255)));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(30, 30, 30, 255)));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(30, 30, 30, 255)));


			ImGui::Button(" ", ImVec2(77, 3));

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

		}
		if (tab == 2)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(255, 42, 0)));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(255, 42, 0)));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(255, 42, 0)));

			ImGui::SameLine();

			ImGui::Button(" ", ImVec2(77, 3));


			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();


		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(30, 30, 30, 255)));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(30, 30, 30, 255)));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(30, 30, 30, 255)));

			ImGui::SameLine();

			ImGui::Button(" ", ImVec2(77, 3));

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

		}
		if (tab == 3)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(255, 42, 0)));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(255, 42, 0)));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(255, 42, 0)));

			ImGui::SameLine();

			ImGui::Button(" ", ImVec2(77, 3));


			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();


		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(30, 30, 30, 255)));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(30, 30, 30, 255)));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(30, 30, 30, 255)));

			ImGui::SameLine();

			ImGui::Button(" ", ImVec2(77, 3));

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		ImGui::BeginChild(("##backro3und"), ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);
		{
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::SameLine();


			if (tab == 1) 			//Aimbot Tab
			{
				CenteredText("Aimbot");
				ImGui::Checkbox("Aim toggle", &Config::toggleAimbot);
				if (Config::toggleAimbot)
				{
					ImGui::Spacing();
					ImGui::Combo("Aim Key", &Vars::selection, Vars::keys, IM_ARRAYSIZE(Vars::keys), 4);
					ImGui::SliderFloat("Smoothing", &Vars::smoothing, 1.f, 1000.f);
				}

			}

			if (tab == 2)			// Visuals Tab
			{
				CenteredText("Visuals");
				ImGui::Checkbox("Outlines", &Config::toggleOutlines);
				if (Config::toggleOutlines)
				{
					ImGui::ColorEdit3("Outline Colours", Vars::colour);
					ImGui::Checkbox("Rainbow Outlines", &Config::rbw);
				}
				ImGui::Spacing();
				ImGui::Checkbox("Radar Hack", &Config::radarHack);
				ImGui::Spacing();
				ImGui::Checkbox("Enable Drawing", &Config::draw); 
				if (Config::draw)
				{
					ImGui::Checkbox("Enable Box ESP", &Config::toggleBoxes); // boxe
					ImGui::Checkbox("Enable Health", &Config::toggleHealth); // boxe

					if (Config::toggleBoxes)
					{
						ImGui::ColorEdit3("Box Colour", Vars::boxCol);
						ImGui::SliderFloat("Box Thickness", &Vars::boxThick, 0.2f, 10.f);
					}
					ImGui::Spacing();

					if (Config::toggleHealth)
					{
						ImGui::ColorEdit3("Health Colour", Vars::textCol);

					}
				}
			}

			if (tab == 3)			// Misc Tab
			{
				CenteredText("Misc");
				ImGui::Checkbox("Bunny Hop", &Config::bhop);
				ImGui::Spacing();
				ImGui::Checkbox("Enable FOV Slider", &Config::fovChanger);

				if (Config::fovChanger)
				{
					ImGui::SliderInt("FOV", &Vars::playerFOV, 0, 360);
				}
				ImGui::Spacing();
				ImGui::Checkbox("Thirdperson", &Config::toggleThird);

			}
			if (tab == 4)
			{

			}

			ImGui::Spacing();


			ImGui::EndChild();

		}


		ImGui::EndChild();
	}

}

void Render() {
	if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	draw();
	ImGui::GetIO().MouseDrawCursor = ShowMenu;

	if (Config::draw)
	{
		boxESP::boxEsp();
	}

	if (ShowMenu == true) {
		InputHandler();


		BeginMenu();


		SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
		UpdateWindow(Overlay.Hwnd);
		SetFocus(Overlay.Hwnd);
	}
	else {
		SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
		UpdateWindow(Overlay.Hwnd);
	}
	ImGui::EndFrame();

	DirectX9.pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (DirectX9.pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		DirectX9.pDevice->EndScene();
	}

	HRESULT result = DirectX9.pDevice->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && DirectX9.pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		DirectX9.pDevice->Reset(&DirectX9.pParameters);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

void MainLoop() {
	static RECT OldRect;
	ZeroMemory(&DirectX9.Message, sizeof(MSG));
	while (DirectX9.Message.message != WM_QUIT) {
		if (PeekMessage(&DirectX9.Message, Overlay.Hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&DirectX9.Message);
			DispatchMessage(&DirectX9.Message);
		}
		HWND ForegroundWindow = GetForegroundWindow();
		if (ForegroundWindow == Process.Hwnd) {
			HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
			SetWindowPos(Overlay.Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT TempRect;
		POINT TempPoint;
		ZeroMemory(&TempRect, sizeof(RECT));
		ZeroMemory(&TempPoint, sizeof(POINT));

		GetClientRect(Process.Hwnd, &TempRect);
		ClientToScreen(Process.Hwnd, &TempPoint);

		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = Process.Hwnd;

		if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
			OldRect = TempRect;
			Process.WindowWidth = TempRect.right;
			Process.WindowHeight = TempRect.bottom;
			DirectX9.pParameters.BackBufferWidth = Process.WindowWidth;
			DirectX9.pParameters.BackBufferHeight = Process.WindowHeight;
			SetWindowPos(Overlay.Hwnd, (HWND)0, TempPoint.x, TempPoint.y, Process.WindowWidth, Process.WindowHeight, SWP_NOREDRAW);
			DirectX9.pDevice->Reset(&DirectX9.pParameters);
		}
		Render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (DirectX9.pDevice != NULL) {
		DirectX9.pDevice->EndScene();
		DirectX9.pDevice->Release();
	}
	if (DirectX9.IDirect3D9 != NULL) {
		DirectX9.IDirect3D9->Release();
	}
	DestroyWindow(Overlay.Hwnd);
	UnregisterClass(Overlay.WindowClass.lpszClassName, Overlay.WindowClass.hInstance);


}

bool DirectXInit() {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9.IDirect3D9))) {
		return false;
	}

	D3DPRESENT_PARAMETERS Params = { 0 };
	Params.Windowed = TRUE;
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Params.hDeviceWindow = Overlay.Hwnd;
	Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	Params.BackBufferWidth = Process.WindowWidth;
	Params.BackBufferHeight = Process.WindowHeight;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.EnableAutoDepthStencil = TRUE;
	Params.AutoDepthStencilFormat = D3DFMT_D16;
	Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	if (FAILED(DirectX9.IDirect3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Overlay.Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9.pDevice))) {
		DirectX9.IDirect3D9->Release();
		return false;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(Overlay.Hwnd);
	ImGui_ImplDX9_Init(DirectX9.pDevice);
	DirectX9.IDirect3D9->Release();
	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message) {
	case WM_DESTROY:
		if (DirectX9.pDevice != NULL) {
			DirectX9.pDevice->EndScene();
			DirectX9.pDevice->Release();
		}
		if (DirectX9.IDirect3D9 != NULL) {
			DirectX9.IDirect3D9->Release();
		}
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (DirectX9.pDevice != NULL && wParam != SIZE_MINIMIZED) {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			DirectX9.pParameters.BackBufferWidth = LOWORD(lParam);
			DirectX9.pParameters.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = DirectX9.pDevice->Reset(&DirectX9.pParameters);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void SetupWindow() {
	Overlay.WindowClass = {
		sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, Overlay.Name, LoadIcon(nullptr, IDI_APPLICATION)
	};

	RegisterClassEx(&Overlay.WindowClass);
	if (Process.Hwnd) {
		static RECT TempRect = { NULL };
		static POINT TempPoint;
		GetClientRect(Process.Hwnd, &TempRect);
		ClientToScreen(Process.Hwnd, &TempPoint);
		TempRect.left = TempPoint.x;
		TempRect.top = TempPoint.y;
		Process.WindowWidth = TempRect.right;
		Process.WindowHeight = TempRect.bottom;
	}

	Overlay.Hwnd = CreateWindowEx(NULL, Overlay.Name, Overlay.Name, WS_POPUP | WS_VISIBLE, Process.WindowLeft, Process.WindowTop, Process.WindowWidth, Process.WindowHeight, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(Overlay.Hwnd, &DirectX9.Margin);
	SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	ShowWindow(Overlay.Hwnd, SW_SHOW);
	UpdateWindow(Overlay.Hwnd);
}

DWORD WINAPI ProcessCheck(LPVOID lpParameter) {
	while (true) {
		if (Process.Hwnd != NULL) {
			if (GetProcessId(TargetProcess) == 0) {
				exit(0);
			}
		}
	}
}

int main() {
	if (CreateConsole == false)
		ShowWindow(GetConsoleWindow(), SW_HIDE);

	bool WindowFocus = false;
	while (WindowFocus == false) {
		DWORD ForegroundWindowProcessID;
		GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
		if (GetProcessId(TargetProcess) == ForegroundWindowProcessID) {
			Process.ID = GetCurrentProcessId();
			Process.Handle = GetCurrentProcess();
			Process.Hwnd = GetForegroundWindow();

			RECT TempRect;
			GetWindowRect(Process.Hwnd, &TempRect);
			Process.WindowWidth = TempRect.right - TempRect.left;
			Process.WindowHeight = TempRect.bottom - TempRect.top;
			Process.WindowLeft = TempRect.left;
			Process.WindowRight = TempRect.right;
			Process.WindowTop = TempRect.top;
			Process.WindowBottom = TempRect.bottom;

			char TempTitle[MAX_PATH];
			GetWindowText(Process.Hwnd, TempTitle, sizeof(TempTitle));
			Process.Title = TempTitle;

			char TempClassName[MAX_PATH];
			GetClassName(Process.Hwnd, TempClassName, sizeof(TempClassName));
			Process.ClassName = TempClassName;

			char TempPath[MAX_PATH];
			GetModuleFileNameEx(Process.Handle, NULL, TempPath, sizeof(TempPath));
			Process.Path = TempPath;

			WindowFocus = true;
		}
	}

	Overlay.Name = RandomString(10).c_str();
	SetupWindow();
	DirectXInit();
	CreateThread(0, 0, ProcessCheck, 0, 0, 0);
	while (TRUE) {
		std::thread t1(glow::outlineESP);
		std::thread t2(fov::playerFOV);
		std::thread t3(chams::playerChams);
		std::thread t4(Aimbot::AimLoop);





		MainLoop();





	}
}
