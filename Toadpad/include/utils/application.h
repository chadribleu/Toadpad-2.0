#ifndef APPLICATION_H
#define APPLICATION_H

#include "common.h"
#include "../utils/acceleratortable.h"
#include "../utils/shortcut.h"
#include "../utils/clipboard.h"
#include "../gui/window.h"

class AcceleratorTable;
class Window;
class Clipboard;

using ICCTYPE = int;
namespace ICCStyle 
{
	static inline const int ANIMATE			= ICC_ANIMATE_CLASS;
	static inline const int BAR				= ICC_BAR_CLASSES;
	static inline const int REBAR			= ICC_COOL_CLASSES;
	static inline const int DATE			= ICC_DATE_CLASSES;
	static inline const int HOTKEY			= ICC_HOTKEY_CLASS;
	static inline const int IPADDRESS		= ICC_INTERNET_CLASSES;
	static inline const int HYPERLINKCTL	= ICC_LINK_CLASS;
	static inline const int LISTVIEW		= ICC_LISTVIEW_CLASSES;
	static inline const int NATIVEFNTCTL	= ICC_NATIVEFNTCTL_CLASS;
	static inline const int PAGESCROLLER	= ICC_PAGESCROLLER_CLASS;
	static inline const int PROGRESSBAR		= ICC_PROGRESS_CLASS;
	static inline const int TABCTL			= ICC_TAB_CLASSES;
	static inline const int TREEVIEW		= ICC_TREEVIEW_CLASSES;
	static inline const int UPDOWNCTL		= ICC_UPDOWN_CLASS;
	static inline const int COMBOBOXEX		= ICC_USEREX_CLASSES;
	static inline const int WIN95STD		= ICC_WIN95_CLASSES;
	static inline const int STANDARD		= ICC_STANDARD_CLASSES;
}

class Application
{
public:
	Application(HINSTANCE instance);
	bool EnableCommonControls(ICCTYPE ctl);
	void Configure(Window& window);
	int Run();

	static HINSTANCE Instance() 
	{ 
		return s_hInst;
	}

	static AcceleratorTable* shortcutTable() 
	{ 
		return &s_accelTable; 
	}

	static Clipboard* clipboard() 
	{ 
		return &s_clpd; 
	}

private:
	static Window* s_mainWnd;
	static AcceleratorTable s_accelTable;
	static Clipboard s_clpd;
	static HINSTANCE s_hInst;
};

#endif