#include "gui/mainwindow.h"

// TODO: Set Edit margin top left to 4

LRESULT CALLBACK MainWindow::s_MainProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (handle == NULL)
		return FALSE;
	MainWindow* pThis = NULL;
	if (msg == WM_NCCREATE) {
		pThis = static_cast<MainWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		pThis->m_self = handle;
		SetWindowLongPtrW(pThis->m_self, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		return DefWindowProcW(handle, msg, wParam, lParam);
	}
	pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	if (pThis) {
		pThis->mParams.wParam = wParam;
		pThis->mParams.lParam = lParam;
		pThis->mParams.uMsg = msg;
		return pThis->HandleMessage(pThis->mParams.uMsg, pThis->mParams.wParam, pThis->mParams.lParam);
	}
	return DefWindowProcW(handle, msg, wParam, lParam);
}

MainWindow::MainWindow(HINSTANCE p_instance, int p_cmdShow) {}
MainWindow::~MainWindow() {}

bool MainWindow::Init()
{
	// Register the main window class
	mClass.cbSize = sizeof(WNDCLASSEX);
	mClass.hInstance = Application::Instance();
	mClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	mClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	mClass.lpfnWndProc = MainWindow::s_MainProc;
	mClass.lpszClassName = getClass();
#ifdef _DEBUG
	mClass.hIcon = LoadIconW(Application::Instance(), MAKEINTRESOURCE(IDI_APPICON_DBG));
	mClass.hIconSm = LoadIconW(Application::Instance(), MAKEINTRESOURCE(IDI_APPICON_DBG_SM));
#else
	m_wcex.hIcon = LoadIcon(Application::Instance(), MAKEINTRESOURCE(IDI_APPICON));
	m_wcex.hIconSm = LoadIcon(Application::Instance(), MAKEINTRESOURCE(IDI_APPICON_SM));
#endif
	if (!RegisterClassExW(&mClass)) {
		return false;
	}
	RECT rc{ 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);
	return create(APPNAME, CW_USEDEFAULT, CW_USEDEFAULT, rc.right, rc.bottom, WS_OVERLAPPEDWINDOW, 0);
}

// --------------- Events
LRESULT MainWindow::OnCreate()
{
	if (!InitConfig()) {
		MessageBoxW(NULL, L"INVALID_SETTINGS", APPNAME, MB_OK);
	}
	InitUI();
	// =========== Set config properties of each controls here ==================================
	m_edit->enableWordWrap(m_Config->Get("editor.enable_word_wrap").as_bool());
	float logicalDeviceUnit = strcmp(m_Config->Get("editor.font_unit").as_string(), "px") == 0 ? 1.0f : 0.75f;
	HFONT hfEdit = CreateFontA(m_Config->Get("editor.font_size").as_float() / logicalDeviceUnit, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		m_Config->Get("editor.font_family").as_string());
	m_edit->SetFont(hfEdit);
	m_statusBar->SetVisibility(m_Config->Get("application.show_status_bar").as_bool());
	m_edit->SetZoom(m_Config->Get("editor.zoom_factor").as_float());
	return static_cast<LRESULT>(0);
}

void MainWindow::InitUI()
{
	m_edit = new Edit(this, 200, 50, L"", WS_VSCROLL | ES_MULTILINE, WS_EX_ACCEPTFILES, IDCTRL_EDIT);
	_tcTest = new TabControl(this, CTCS_CLOSEABLE);
	_tcTest->SetDisplayArea(m_edit);
	// HLOCAL hOldMem = reinterpret_cast<HLOCAL>(m_edit->Notify<WPARAM, LPARAM>(EM_GETHANDLE, 0, 0));
	AddTab(L"Untitled");
	// LocalFree(hOldMem);

	m_statusBar = new StatusBar(this, SBARS_SIZEGRIP);
	std::array<int, 2> arr{ 128, 256 };
	m_statusBar->SetParts(arr.data(), static_cast<int>(arr.size()), false);

	// File
	Shortcut shNew(Shortcut::Modifier::CTRL, 'N');
	Shortcut shOpen(Shortcut::Modifier::CTRL, L'O');
	Shortcut shSave(Shortcut::CTRL, 'S');
	Shortcut shSaveAs(Shortcut::Modifier::CTRL | Shortcut::Modifier::SHIFT, L'S');

	// Edit
	Shortcut shUndo(Shortcut::Modifier::CTRL, 'Z');
	Shortcut shCut(Shortcut::Modifier::CTRL, 'X');
	Shortcut shCopy(Shortcut::Modifier::CTRL, 'C');
	Shortcut shPaste(Shortcut::Modifier::CTRL, 'V');
	Shortcut shSelAll(Shortcut::Modifier::CTRL, 'A');

	// Format
	Shortcut shZoomIn(Shortcut::Modifier::CTRL, VK_ADD);
	Shortcut shZoomOut(Shortcut::Modifier::CTRL, VK_SUBTRACT);
	Shortcut shRestoreZoom(Shortcut::Modifier::CTRL, VK_NUMPAD0);

	// File menu
	m_menuBar = new MenuBar(this);

	m_fileMenu = new Menu;
	m_itmNew = m_fileMenu->AddItem(L"New", IDM_NEW, shNew);

	m_itmOpen = m_fileMenu->AddItem(L"Open", IDM_OPEN, shOpen);
	m_itmSave = m_fileMenu->AddItem(L"Save", IDM_SAVE, shSave);
	m_itmSaveAs = m_fileMenu->AddItem(L"Save as...", IDM_SAVEAS, shSaveAs);

	m_fileMenu->AddSeparator();

	m_itmExit = m_fileMenu->AddItem(L"Close", IDM_EXIT);

	// Edit menu
	m_editMenu = new Menu;
	m_itmUndo = m_editMenu->AddItem(L"Undo", IDM_UNDO, shUndo);

	m_editMenu->AddSeparator();

	m_itmCut = m_editMenu->AddItem(L"Cut", IDM_CUT, shCut);
	m_itmCopy = m_editMenu->AddItem(L"Copy", IDM_COPY, shCopy);
	m_itmPaste = m_editMenu->AddItem(L"Paste", IDM_PASTE, shPaste);
	m_itmDelete = m_editMenu->AddItem(L"Delete", IDM_DELETE);

	m_editMenu->AddSeparator();

	m_itmSelAll = m_editMenu->AddItem(L"Select All", IDM_SELALL, shSelAll);

	m_formatMenu = new Menu;
	m_itm_wordWrap = m_formatMenu->AddItem(L"Word Wrap", IDM_ENABLEWORDWRAP);
	m_itm_changeFont = m_formatMenu->AddItem(L"Font...", IDM_SETFONT);

	m_viewMenu = new Menu;
	m_zoomMenu = new Menu;
	m_viewMenu->AddMenu(L"Zoom", *m_zoomMenu);
	m_itmZoomIn = m_zoomMenu->AddItem(L"Zoom In", IDM_ZOOMIN, shZoomIn);
	m_itmZoomOut = m_zoomMenu->AddItem(L"Zoom Out", IDM_ZOOMOUT, shZoomOut);
	m_itmRestoreZoom = m_zoomMenu->AddItem(L"Restore Default Zoom", IDM_RESTOREZOOM, shRestoreZoom);
	m_itmShowStatusBar = m_viewMenu->AddItem(L"Show Status Bar", IDM_ENABLESTATUSBAR);
	m_itmShowStatusBar->SetChecked(m_statusBar->Visible());

	m_menuBar->AddMenu(L"File", m_fileMenu);
	m_menuBar->AddMenu(L"Edit", m_editMenu);
	m_menuBar->AddMenu(L"Format", m_formatMenu);
	m_menuBar->AddMenu(L"View", m_viewMenu);
}

void MainWindow::OnCommandNew()
{
	AddTab(std::filesystem::path(), nullptr);
}

void MainWindow::OnCommandOpen()
{
	FilePicker fp(m_self, FilePicker::Mode::OPEN);
	fp.SetFilters(L"All files (*.*)|*.*|Text files (*.txt)|*.txt", 2);
	
	if (fp.ShowDialog(L"C:\\Users\\AdriA\\Downloads")) {
		DocumentHandler dc(fp.GetResult());
		std::wstring sContent = dc.ReadFile();
		size_t size = sContent.capacity();
		HLOCAL mem = LocalAlloc(LHND, size * sizeof(wchar_t));
		auto dest = static_cast<wchar_t*>(LocalLock(mem));
		Utils::StringCopy(dest, sContent.c_str(), static_cast<int>(size));
		LocalLock(mem);
		this->AddTab(dc.GetFileName().c_str(), mem);
	}
}

// OnDocumentSave(PromptMode promptDlg) == ALWAYS;NEVER;AUTO
void MainWindow::OnDocumentSave(bool always_open_dlg)
{
	int docIndex = _tcTest->SelectedIndex();
	DocumentHandler* currentDocument = &m_docList.at(docIndex).docHandler;
	std::wstring associatedPath = currentDocument->GetPath();
	if ((!m_docList.at(docIndex).docHandler.GetPath().empty()) && (!always_open_dlg)) {
		if (currentDocument->createFile(associatedPath)) {
			currentDocument->WriteOnFile(m_edit->getText());
			m_docList[docIndex].docHandler.SetDirty(false);
			return;
		}
	}
	try {
		FilePicker cstmFilePicker(this->getHandle(), FilePicker::Mode::SAVE);

		cstmFilePicker.SetFilters(L"Text files (*.txt)|*.txt|All files (*.*)|*.*");
		if (cstmFilePicker.ShowDialog(L"C:\\MyFile")) {
			std::wstring test = cstmFilePicker.GetResult();
			if (currentDocument->createFile(cstmFilePicker.GetResult())) {
				currentDocument->WriteOnFile(m_edit->getText());
				m_docList[docIndex].docHandler.SetDirty(false);
			}
			else {
				MessageBox(this->getHandle(), L"Unable to save your file", APPNAME, MB_OK | MB_ICONERROR);
			}
		}
	}
	catch (const std::exception& e) {
		MessageBoxA(m_self, e.what(), "Error", MB_OK | MB_ICONERROR);
	}
}

void MainWindow::AddTab(std::filesystem::path const& path, HLOCAL content)
{
	m_docList.emplace_back(
		DocumentHandler(path), content == nullptr ? LocalAlloc(LHND, 32000) : 
		content, 
		Edit::SelectionRange {.begin = 0, .end = 0 }
	);

	const uint32_t tabItemCount = _tcTest->GetItemCount();
	m_edit->swapMem(m_docList[tabItemCount].associatedBuffer);
	_tcTest->InsertItem(path.empty() ? L"Untitled" : path.wstring().c_str());
	_tcTest->Goto(tabItemCount);
}

// mDocList >> out of range (because NextIndex >)
bool MainWindow::CloseTab(int index)
{
	int selectedIndex = _tcTest->SelectedIndex();
	int tabItemCount = _tcTest->GetItemCount();
	UINT nextIndex = selectedIndex;

	try {
		if (index < selectedIndex || index == selectedIndex && index == (tabItemCount - 1)) --nextIndex;
		else if (index == selectedIndex && index != 0 && index + 1 < (tabItemCount - 1)) ++nextIndex;
		if (LocalFree(m_docList[index].associatedBuffer) != nullptr) return false;
		m_docList.erase((m_docList.begin()) + index);
		_tcTest->RemoveItem(index);
		if (tabItemCount - 1 > 0) {
			_tcTest->Goto(nextIndex);
			m_edit->swapMem(m_docList.at(nextIndex).associatedBuffer);
			return true;
		}
		else {
			Notify(WM_CLOSE, 0, 0);
		}
		return false;
	}
	catch (const std::exception& e) {
		MessageBox(m_self, L"An exception has occured.", APPNAME, MB_OK);
	}
}

bool MainWindow::InitConfig()
{
	const size_t size = MAX_PATH;
	auto pPath = std::bit_cast<char*>(CoTaskMemAlloc(sizeof(char) * size));
	if (SHGetSpecialFolderPathA(nullptr, pPath, CSIDL_LOCAL_APPDATA, FALSE) == FALSE) {
		return false;
	}
	strcat_s(pPath, size, R"(\chadribleu.toadpad\config.xml)");
	std::filesystem::path configPath(pPath);
	CoTaskMemFree(pPath);
	m_Config = ConfigurationManager::GetInstance();
	if (!m_Config->Load(configPath)) {
		return m_Config->Create(pPath);
	}
	return true;
}

LRESULT MainWindow::OnResize(UINT width, UINT height)
{
	int sHeight = 0;
	if (m_statusBar->Visible()) {
		m_statusBar->SizeChanged();
		RECT statusBarRect;
		GetWindowRect(m_statusBar->getHandle(), &statusBarRect);
		sHeight = (statusBarRect.bottom - statusBarRect.top);
	}
	_tcTest->setGeometry(PosInfo{ -1, -1, (int)width, (int)height - sHeight });
	return 0;
}

LRESULT MainWindow::OnCommand()
{
	switch (LOWORD(mParams.wParam))
	{
	case IDM_NEW:
		OnCommandNew();
		break;
	case IDM_OPEN:
		OnCommandOpen();
		break;
	case IDM_SAVE:
		OnDocumentSave(false);
		break;
	case IDM_SAVEAS:
		OnDocumentSave(true);
		break;
	case IDM_EXIT:
		Notify(WM_CLOSE, 0, 0);
		break;
	case IDM_UNDO:
		m_edit->Notify<WPARAM, LPARAM>(EM_UNDO);
		break;
	case IDM_CUT:
	{
		if (Edit::SelectionRange sel = m_edit->getSelection(); (sel.end - sel.begin) <= 0) {
			break;
		}
		Clipboard* clpd = Application::clipboard();
		clpd->Open();
		clpd->SetText(m_edit->GetSelText().c_str());
		clpd->Close();
		m_edit->Notify(EM_REPLACESEL, TRUE, L"");
		break;
	}
	case IDM_COPY:
	{
		if (Edit::SelectionRange sel = m_edit->getSelection(); (sel.end - sel.begin) > 0) {
			Clipboard* clpd = Application::clipboard();
			clpd->Open();
			clpd->SetText(m_edit->GetSelText().c_str());
			clpd->Close();
		}
		break;
	}
	case IDM_PASTE:
	{
		Clipboard* clpd = Application::clipboard();
		clpd->Open();
		m_edit->ReplaceSel(clpd->GetText());
		clpd->Close();
		break;
	}
	case IDM_DELETE:
		m_edit->ReplaceSel(L"");
		break;
	case IDM_SELALL:
		m_edit->SetSelection(Edit::SelectionRange{ 0, -1 });
		break;
	case IDM_ENABLEWORDWRAP:
	{
		bool check = m_itm_wordWrap->IsChecked();
		m_edit->enableWordWrap(!check);
		m_itm_wordWrap->SetChecked(!check);
		break;
	}
	case IDM_SETFONT:
	{
		if (FontPicker fp(getHandle()); fp.ShowDialog()) {
			m_edit->SetFont(fp.GetFont());
		}
		break;
	}
	case IDM_ENABLESTATUSBAR:
	{
		bool menu_check = m_statusBar->SetVisibility(!m_statusBar->Visible());
		m_itmShowStatusBar->SetChecked(!menu_check);
		RECT window_size = { 0 };
		RECT status_height = { 0 };
		POINT coordinates = _tcTest->getLocalCoordinates();
		GetWindowRect(_tcTest->getHandle(), &window_size);
		GetWindowRect(m_statusBar->getHandle(), &status_height);
		if (!menu_check == true) {
			_tcTest->setGeometry(PosInfo{ coordinates.x, coordinates.y,
				window_size.right - window_size.left, (window_size.bottom - window_size.top) - (status_height.bottom - status_height.top) });
		}
		else {
			_tcTest->setGeometry(PosInfo{ coordinates.x, coordinates.y,
				window_size.right - window_size.left, (window_size.bottom - window_size.top) + (status_height.bottom - status_height.top) });
		}
		break;
	}
	case IDM_ZOOMIN:
		m_edit->SetZoom(10);
		break;
	case IDM_ZOOMOUT:
		m_edit->SetZoom(-10);
		break;
	case IDM_RESTOREZOOM:
		m_edit->SetZoom(0);
		break;

	case IDCTRL_LISTBOX:
		switch (HIWORD(mParams.wParam)) {
		case CBN_SELCHANGE:
			MessageBox(this->getHandle(), L"Sel changed - ListBoxMessage", L"Title", MB_OK);
			break;
		}
		break;
	}
	return static_cast<LRESULT>(0);
}

LRESULT MainWindow::OnCustomCommand()
{
	switch (mParams.uMsg)
	{
	case CTM_ONCOMMAND:
	{
		switch (LOWORD(mParams.wParam))
		{
		case IDCTRL_EDIT:
			m_docList[_tcTest->SelectedIndex()].docHandler.SetDirty(true);
			break;
		}
		break;
	}
	case CEM_GETLINEINFO:
	{
		const size_t buffSize = 24;
		std::wstring buffer(buffSize, L'\0');

		if (int count = _snwprintf_s(buffer.data(), buffer.size(), _TRUNCATE, L" Ln %d, Col %d", LOWORD(mParams.wParam), HIWORD(mParams.wParam)); count != -1)
		{
			m_statusBar->SetText(buffer.c_str(), 0);
		}
		break;
	}
	case CEM_ZOOMCHANGED:
	{
		const int maxSize = 5;
		auto buffer = new wchar_t[maxSize];
		m_statusBar->SetText(buffer, 1);
		delete[] buffer;
		break;
	}
	}
	return 0;
}

LRESULT MainWindow::OnNotify()
{
	switch (std::bit_cast<NMHDR*>(mParams.lParam)->code)
	{
	case CTCN_SELCHANGE:
	{
		int selIndex = _tcTest->SelectedIndex();
		m_docList[static_cast<unsigned int>(mParams.wParam)].selection = m_edit->getSelection();
		m_edit->swapMem(m_docList[selIndex].associatedBuffer);
		m_edit->SetSelection(m_docList[selIndex].selection);
		break;
	}
	case CTCN_ADDBUTTONPRESSED:
	{
		AddTab(L"Untitled");
		break;
	}
	case CTCN_CLOSEBUTTONPRESSED:
		CloseTab(static_cast<int>(mParams.wParam));
		break;
	}
	return 0;
}

LRESULT MainWindow::OnClose()
{
	std::vector<unsigned> at;
	const unsigned remaining = _tcTest->GetItemCount();
	int dirty = 0;
	for (int i = 0; i < remaining; ++i) {
		if (m_docList[i].docHandler.Dirty()) {
			++dirty;
			at.push_back(dirty);
		}
	}
	if (dirty > 0) {
		int result = MessageBoxW(this->getHandle(),
			L"You're about to quit Toadpad with some unsaved changes\nDo you want to save them?",
			APPNAME, MB_YESNO);
		if (result == IDYES) {
			for (unsigned i = 0; i < remaining; ++i) {
				_tcTest->Goto((at[i]));
				this->OnDocumentSave(false);
			}
		}
	}
	DestroyWindow(m_self);
	return 0;
}

LRESULT MainWindow::OnDestroy()
{
	PostQuitMessage(GetLastError());
	return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:			return OnCreate();
	case WM_SIZE:			return OnResize(LOWORD(lParam), HIWORD(lParam));
	case WM_COMMAND:		return OnCommand();
	case WM_NOTIFY:			return OnNotify();
	case CEM_GETLINEINFO:
	case CEM_ZOOMCHANGED:
	case CTM_ONCOMMAND:
		return OnCustomCommand();
	case WM_CLOSE:			return OnClose();
	case WM_DESTROY:		return OnDestroy();
	default: return DefWindowProc(m_self, uMsg, wParam, lParam);
	}
}