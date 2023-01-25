#ifndef MAINWINDOW
#define MAINWINDOW

#include "constants.h"

// Chadribleu.Dialogs
#include "FilePicker.h"
#include "FontPicker.h"

#include "window.h"
#include "edit.h"
#include "statusbar.h"
#include "menu.h"
#include "ListBox.h"
#include "resource.h"

// NO GUI
#include "../utils/shortcut.h"
#include "../utils/documenthandler.h"
#include "../utils/configurationmanager.h"

// Custom control
#include "tabcontrol.h"

class Edit;
class DocumentHandler;

class MainWindow : public Window 
{
private:
	struct DocList {
		DocumentHandler docHandler;
		HLOCAL associatedBuffer;
		Edit::SelectionRange selection;
	};
	struct Parameters {
		UINT uMsg;
		WPARAM wParam;
		LPARAM lParam;
	};

	static LRESULT CALLBACK s_MainProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT HandleMessage(UINT, WPARAM, LPARAM);

	// Controls
	TabControl* _tcTest = nullptr;
	Edit* m_edit = nullptr;
	StatusBar* m_statusBar = nullptr;
	MenuBar* m_menuBar = nullptr;

	// test
	ListBox* m_ListBox = nullptr;

	Menu* m_fileMenu = nullptr;
	MenuItem* m_itmNew = nullptr;
	MenuItem* m_itmOpen = nullptr;
	MenuItem* m_itmSave = nullptr;
	MenuItem* m_itmSaveAs = nullptr;
	MenuItem* m_itmExit = nullptr;

	Menu* m_editMenu = nullptr;
	MenuItem* m_itmUndo = nullptr;
	MenuItem* m_itmCut = nullptr;
	MenuItem* m_itmCopy = nullptr;
	MenuItem* m_itmPaste = nullptr;
	MenuItem* m_itmDelete = nullptr;
	MenuItem* m_itmSelAll = nullptr;

	Menu* m_formatMenu = nullptr;
	MenuItem* m_itm_wordWrap = nullptr;
	MenuItem* m_itm_changeFont = nullptr;

	Menu* m_viewMenu = nullptr;
	Menu* m_zoomMenu = nullptr;
	MenuItem* m_itmZoomIn = nullptr;
	MenuItem* m_itmZoomOut = nullptr;
	MenuItem* m_itmRestoreZoom = nullptr;
	MenuItem* m_itmShowStatusBar = nullptr;
	
	ConfigurationManager* m_Config = nullptr;
	Parameters mParams;
	std::vector<DocList> m_docList{0};

	// Messages
	void InitUI();
	void OnCommandNew();
	void OnCommandOpen();
	void OnDocumentSave(bool always_open_dlg);

	// test
	LRESULT OnCreate();

	LRESULT OnCommand();
	LRESULT OnResize(UINT width, UINT height);
	LRESULT OnCustomCommand();
	LRESULT OnNotify();
	LRESULT OnClose();
	LRESULT OnDestroy();

	void AddTab(std::filesystem::path const& path, HLOCAL content = nullptr);
	bool CloseTab(int index);
	bool InitConfig();

	WNDCLASSEX mClass{0};

public:
	MainWindow(HINSTANCE p_instance, int p_cmdShow);
	~MainWindow();
	bool Init();

#ifdef _WIN64
	inline const wchar_t* getClass() override { return L"chadribleu.toadpad_x64"; }
#else
	inline const wchar_t* getClass() override { return L"chadribleu.toadpad_x86"; }
#endif
};

#endif