// -------------------------------------------------------------------------------------------
// Toadpad 2.0 entry point - See other files for more information
// -------------------------------------------------------------------------------------------

#include <stdlib.h>
#include <crtdbg.h>

#include "gui/mainwindow.h"
#include "utils/application.h"

#include "utils/configurationmanager.h"

#include <cassert>

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE UNREFERENCED_PARAMETER(hPrevInstance), LPWSTR pCmdLine, int nCmdShow) 
{
	Application app(hInst);
	app.EnableCommonControls(ICCStyle::STANDARD | ICCStyle::WIN95STD);

	MainWindow toadpadWindow(hInst, nCmdShow);
	toadpadWindow.Init();

	app.Configure(toadpadWindow);
	return app.Run();
}