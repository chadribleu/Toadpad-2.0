#include "utils/application.h"

HINSTANCE Application::s_hInst = NULL;
Window* Application::s_mainWnd = nullptr;
AcceleratorTable Application::s_accelTable = AcceleratorTable();
Clipboard Application::s_clpd;

void Application::Configure(Window& window) 
{
	s_mainWnd = &window;
	s_clpd = Clipboard(&window);
	s_accelTable.Create();
}

int Application::Run() 
{
	MSG message = { 0 };
	HWND hMainWindow = s_mainWnd->getHandle();
	HACCEL hAcc = s_accelTable.GetHandle();

	while (GetMessageW(&message, NULL, NULL, NULL) > 0) {
		if (!TranslateAcceleratorW(hMainWindow, hAcc, &message)) {
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}
	return static_cast<int>(message.wParam);
}

Application::Application(HINSTANCE instance)
{
	s_hInst = instance;
}

bool Application::EnableCommonControls(ICCTYPE ctl) 
{
	INITCOMMONCONTROLSEX iccex;
	iccex.dwSize = sizeof(iccex);
	iccex.dwICC = ctl;

	if (InitCommonControlsEx(&iccex) == FALSE) {
		return false;
	}
	return true;
}