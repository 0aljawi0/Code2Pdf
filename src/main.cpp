#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "ui/Theme.h"
#include "ui/AppWindow.h"
#include "platform/NullDialogService.h"

#ifdef _WIN32
#include "platform/Win32DialogService.h"
#define NOMINMAX
#include <windows.h>
#include <dwmapi.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#ifdef _WIN32
#include <shobjidl.h>
#endif

#include <cstdio>

#ifdef _WIN32
namespace
{
	// Windows 11 native decoration:
	//   * rounded corners
	//   * dark title bar
	//   * title bar / border / text colours matching the dark Spectrum theme.
	// Unsupported attributes simply fail on Windows 10 and the classic frame
	// is used instead.
	void ApplyNativeWindowsDecoration(GLFWwindow* window)
	{
		HWND hwnd = glfwGetWin32Window(window);
		if (!hwnd) return;

		BOOL useDark = TRUE;
		if (FAILED(DwmSetWindowAttribute(hwnd, 20, &useDark, sizeof(useDark))))
			DwmSetWindowAttribute(hwnd, 19, &useDark, sizeof(useDark));

		DWORD corner = 2; // DWMWCP_ROUND
		DwmSetWindowAttribute(hwnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/,
			&corner, sizeof(corner));

		COLORREF caption = RGB(34, 34, 34);    // GRAY100 (window bg)
		COLORREF text = RGB(227, 227, 227); // GRAY800 (text)
		COLORREF border = RGB(58, 58, 58);    // GRAY300 (border)
		DwmSetWindowAttribute(hwnd, 35 /*DWMWA_CAPTION_COLOR*/, &caption, sizeof(caption));
		DwmSetWindowAttribute(hwnd, 36 /*DWMWA_TEXT_COLOR*/, &text, sizeof(text));
		DwmSetWindowAttribute(hwnd, 34 /*DWMWA_BORDER_COLOR*/, &border, sizeof(border));
	}

	void ApplyWindowIcon(GLFWwindow* window)
	{
		HWND hwnd = glfwGetWin32Window(window);
		if (!hwnd) return;

		HINSTANCE hInst = GetModuleHandleW(nullptr);

		// Load the icons using the integer ID (101) defined in resources.rc
		HICON big_icon = (HICON)LoadImageW(
			hInst,
			MAKEINTRESOURCEW(101), // Forces Windows to look for resource ID 101
			IMAGE_ICON,
			0, 0,
			LR_DEFAULTSIZE | LR_SHARED
		);

		HICON small_icon = (HICON)LoadImageW(
			hInst,
			MAKEINTRESOURCEW(101),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_SHARED);

		if (big_icon) SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)big_icon);
		if (small_icon) SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)small_icon);
	}
}
#endif

int main(int, char**)
{
#ifdef _WIN32
	SetCurrentProcessExplicitAppUserModelID(L"Code2Pdf.App");
#endif

	glfwSetErrorCallback([](int code, const char* desc) {
		fprintf(stderr, "GLFW error %d: %s\n", code, desc);
	});

	if (!glfwInit()) return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	int windowWidth = 1100;
	int windowHeight = 760;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	int xPos = (mode->width - windowWidth) / 2;
	int yPos = (mode->height - windowHeight) / 2;

	glfwWindowHint(GLFW_POSITION_X, xPos);
	glfwWindowHint(GLFW_POSITION_Y, yPos);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Code2Pdf", nullptr, nullptr);
	if (!window) { glfwTerminate(); return 1; }

#ifdef _WIN32
	ApplyNativeWindowsDecoration(window);
	ApplyWindowIcon(window);
#endif

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;
	code2pdf::Theme::Apply();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 150");

#ifdef _WIN32
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	code2pdf::Win32DialogService dialogs;
#else
	code2pdf::NullDialogService dialogs;
#endif

	code2pdf::AppWindow app(dialogs);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		app.Draw();

		ImGui::Render();
		int w = 0, h = 0;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);
		glClearColor(0.133f, 0.133f, 0.133f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

#ifdef _WIN32
	CoUninitialize();
#endif

	return 0;
}