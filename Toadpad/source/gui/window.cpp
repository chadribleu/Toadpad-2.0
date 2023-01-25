#include "gui/window.h"

// TODO: Rewrite all functions with Notify() instead of SendMessage

Window::Window() : m_self(NULL), m_parent(NULL), m_menu(NULL) {}

Window::~Window() 
{
	DestroyWindow(m_self);
}

void Window::setMenuBar(MenuBar* bar) 
{
	HMENU assignedMenu = GetMenu(m_self);
	if (assignedMenu != NULL) {
		DestroyMenu(assignedMenu);
	}
	m_menu = bar->GetHandle();
	SetMenu(m_self, m_menu);
}

HWND Window::BaseParent() const
{
	HWND hParent = GetAncestor(m_parent, GA_PARENT);
	return hParent == GetDesktopWindow() ? m_parent : hParent;
}

void Window::setText(const wchar_t* text)
{
	SendMessage(m_self, WM_SETTEXT, 0, (LPARAM)text);
}

// Note: Don't forget to free the memory with delete
wchar_t* Window::getText() 
{
	uint32_t tsz = GetWindowTextLength(m_self) + 1;
	wchar_t* str = new wchar_t[tsz];
	GetWindowTextW(m_self, str, tsz);
	return str;
}

bool Window::create(const wchar_t* title, int x, int y, int width, int height, DWORD style, DWORD exStyle, Window* parent, HMENU menu) 
{
	m_style = style;
	m_exStyle = exStyle;
	
	bool isChild = false;
	if (parent != nullptr) {
		isChild = true;
		m_parent = parent->getHandle();
		m_style |= WS_CHILD;
	}
	if (menu != NULL) m_menu = menu;

	m_self = CreateWindowExW(m_exStyle, this->getClass(), title, m_style, x, y, width, height,
		isChild ? parent->getHandle() : NULL, menu, Application::Instance(), this);
	m_fnt = reinterpret_cast<HFONT>(SendMessage(m_self, WM_GETFONT, 0, 0));

	if (m_self != NULL) {
		m_visible = true;
		ShowWindow(m_self, SW_SHOW);
		UpdateWindow(m_self);
		return true;
	} 
	else {
		m_visible = false;
		return false;
	}
}

bool Window::SetVisibility(int state)
{
	m_visible = state > SW_HIDE ? true : false;
	return static_cast<bool>(ShowWindow(m_self, state));
}

// TODO: Avoid the if statement
bool Window::setGeometry(PosInfo const& ps) 
{
	UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER;
	if (ps.x < NULL || ps.y < NULL) {
		flags |= SWP_NOMOVE;
	}
	if (ps.width == -1 || ps.height == -1) {
		flags |= SWP_NOSIZE;
	}
	return SetWindowPos(m_self, NULL, ps.x, ps.y, ps.width, ps.height, flags);
}

POINT Window::getLocalCoordinates() const
{
	RECT clientAreaSz{ 0 };
	GetClientRect(m_self, &clientAreaSz);
	POINT pt{ clientAreaSz.left, clientAreaSz.top };
	MapWindowPoints(m_self, GetParent(m_self), &pt, 1);
	return pt;
}

bool Window::repaint(DWORD style, DWORD exStyle, HWND parent)
{
	// TODO EDIT: Delete not assigned HLOCAL memory
	const int txtSize = GetWindowTextLengthW(m_self) + 1;
	wchar_t* prevWndText = new wchar_t[txtSize];
	if (prevWndText != nullptr) {
		GetWindowTextW(m_self, prevWndText, txtSize);
	}
	int winID = 0;
	if ((style & WS_CHILD) != NULL) {
		int return_val = GetDlgCtrlID(m_self);
		if (return_val > 0) {
			winID = return_val;
		}
	}
	RECT size{ 0 };
	GetWindowRect(m_self, &size);

	POINT pos = this->getLocalCoordinates();

	HWND hTmpWnd = CreateWindowExW(exStyle, this->getClass(), prevWndText, style,
		pos.x, pos.y, size.right - size.left, size.bottom - size.top,
		parent, winID > 0 ? reinterpret_cast<HMENU>(winID) : m_menu, Application::Instance(), this);
	
	delete[] prevWndText;

	if (hTmpWnd != NULL) {
		DestroyWindow(m_self);

		m_self = hTmpWnd;
		m_parent = parent;
		m_style = style;
		m_exStyle = exStyle;

		this->SetFont(m_fnt);

		if (m_proc != nullptr) {
			this->Subclass(m_proc, m_procId);
		}
		
		ShowWindow(m_self, SW_SHOW);
		UpdateWindow(m_self);
		return true;
	}
	return false;
}

uint32_t Window::getTextLength() 
{
	const int sz = GetWindowTextLengthW(m_self);
	wchar_t* tmpBuff = new wchar_t[sz];
	unsigned long buffSize = (int)SendMessageW(m_self, WM_GETTEXT, (WPARAM)sz, (LPARAM)tmpBuff);
	delete[] tmpBuff;

	return buffSize;
}

void Window::SetFont(HFONT font) 
{ 
	SendMessage(m_self, WM_SETFONT, reinterpret_cast<WPARAM>(font), static_cast<LPARAM>(TRUE));
	m_fnt = font;
}

bool Window::Subclass(SUBCLASSPROC new_proc, UINT_PTR proc_id)
{
	m_proc = new_proc;
	m_procId = proc_id;
	if (SetWindowSubclass(m_self, m_proc, m_procId, reinterpret_cast<DWORD_PTR>(this)) == TRUE) {
		this->Notify(CWM_SUBCLASSED, MAKEWPARAM(this, new_proc), 0);
		return true;
	}
	return false;
}