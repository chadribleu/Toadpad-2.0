#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"
#include "constants.h"
#include "../utils/application.h"
#include "../utils/shortcut.h"
#include "menu.h"
#include "menubar.h"
#include "menuitem.h"

// TODO: Better window subclassing
// TODO: Update code for each window (Subclass() function...

class MenuBar;

constexpr size_t DEFWIDTH = 250;
constexpr size_t DEFHEIGHT = 350;
constexpr size_t WINDOWCLASS_USEDEFAULT = -1;

struct SizePolicy 
{
	int minimum_width;
	int minimum_height;
};

class Window 
{
protected:
	bool create(const wchar_t* title,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
		int width = DEFWIDTH, int height = DEFHEIGHT,
		DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0, Window* parent = nullptr,
		HMENU menu = nullptr);

	SizePolicy m_SizePolicy;

	HWND m_self;
	HWND m_parent;
	HFONT m_fnt;
	HMENU m_menu;
	DWORD m_style;
	DWORD m_exStyle;
	bool m_visible;

	SUBCLASSPROC m_proc;
	UINT_PTR m_procId;
public:
	struct PosInfo {
		int x, y, width, height;
	};

	Window();
	Window(Window const&) = delete;
	virtual ~Window();
	
	const Window& operator=(const Window&) = delete;

	virtual const wchar_t* getClass() { return L"Window"; }

	void setMenuBar(MenuBar* bar);

	inline HWND getHandle() const { return m_self; }
	inline HWND getParent() const { return m_parent; }
	HWND BaseParent() const;

	void setText(const wchar_t* text);
	wchar_t* getText();

	bool SetVisibility(int state);
	bool Visible() const { return m_visible; }
	bool setGeometry(PosInfo const& infos);
	POINT getLocalCoordinates() const;

	// TODO: Menu or keep ID
	bool repaint(DWORD style, DWORD exStyle, HWND parent);

	uint32_t getTextLength();
	void SetFont(HFONT font);
	
	bool Subclass(SUBCLASSPROC new_proc, UINT_PTR proc_id);

	template<class T_WPARAM, class T_LPARAM>
	LRESULT Notify(UINT message, T_WPARAM wParam = 0, T_LPARAM lParam = 0) 
	{
		return SendMessage(m_self, message, (WPARAM)wParam, (LPARAM)lParam);
	}
};

#endif