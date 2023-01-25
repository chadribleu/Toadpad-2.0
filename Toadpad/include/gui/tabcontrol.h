#ifndef TABCONTROL_H
#define TABCONTROL_H

#include "common.h"
#include "window.h"

#define TC_BEGINMESSAGECODE	(WM_USER + 0x0040)
#define CTCM_ONCOMMAND (TC_BEGINMESSAGECODE + 0x0001)
#define CTCM_NOMORETABS	(TC_BEGINMESSAGECODE + 0x0002)

#define TC_BEGINNOTIFICATION (WM_USER + 0x0010)
#define CTCN_ADDBUTTONPRESSED (WM_USER + TC_BEGINNOTIFICATION + 0x0001)
#define CTCN_SELCHANGE (TC_BEGINNOTIFICATION + 0x0002)
#define CTCN_CLOSEBUTTONPRESSED	(TC_BEGINNOTIFICATION + 0x0003)
#define CTCN_TABSWITCH (TC_BEGINNOTIFICATION + 0x0004)

#define CTCS_CLOSEABLE 0x0001

using namespace Gdiplus;

class TabControl : public Window
{
private:
	bool bDoubleBuffering = true;
	// Enums
	enum class CommButtonState : int {
		Clicked = 0,
		Hovered,
		Normal,
		Disabled,
		STATE_COUNT
	};
	enum class ResourceType {
		Tab,
		CloseButton,
		AddButton
	};
	enum class DragRequest {
		Accepted,
		Pending,
		Ask,
		None
	};
	// Structures declaration
	struct ColorPalette {
		ARGB fkgnd_active;
		ARGB fkgnd_inactive;
		ARGB fkgnd_hover;
		ARGB bkgnd;
		ARGB outline;
		ARGB hover;
		ARGB selected;
		ARGB normal;
		ARGB disabled;
		ARGB userAccentColor;
	};
	struct Item {
		UINT uMask;
		wchar_t* pszLabel;
		size_t cSize;
		unsigned int iIndex;
		RECT rcBounds;
		int maximumWidth = 150;
	};
	struct DraggedItem {
		int which = -1;
		POINT delta;
		HBITMAP cache;
		RECT posAtScreen;
		RECT previousRect;
		DragRequest dragRequest = DragRequest::None;
	};
	struct Tabs {
		std::vector<Item> item;
		int count;
		BOOL trackMouse;
		int selectedIndex;
		int prevSelectedIndex;
		int hoveredIndex;
		int prevHoveredIndex;
		ColorPalette palette;
		int minimumWidth = 35;
		int maximumHeight = 35;
		// Item dragging
		DraggedItem draggedItem;
		bool isItemDragged = false;
	};
	struct CommTabButton {
		CommButtonState state;
		Bitmap* gfx[static_cast<int>(CommButtonState::STATE_COUNT)];
	};
	struct ItemSkeleton {
		UINT index;
		Graphics* graphics;
		const Rect& bounds;
	};

	Window* _hDsp;
	Tabs mTabsInfo;
	ColorPalette mPalette;
	CommTabButton mCloseButton;
	CommTabButton mAddButton;

	// window class management
	BOOL CUnregisterClass();
	static LRESULT s_TCproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(UINT, WPARAM, LPARAM);

	// gdiplus
	GdiplusStartupInput mGdiGsi;
	UINT_PTR mGdiToken;

	// initialization
	void InitButtons();
	void InitPalette(ColorPalette& palette);

	bool ResizeItem(UINT index, int cx, int cy);
	void DrawItem(ItemSkeleton& drawInfo, Gdiplus::Font const& font, Gdiplus::StringFormat const& format);
	void DrawCloseButton(ItemSkeleton& drawInfo);
	void DrawAddTabButton(HDC hdc);
	
	// todo: helpers
	Color Blend(Gdiplus::ARGB color, Gdiplus::ARGB background);
	void AdjustBrightness(Gdiplus::Color& source, float value);
	
public:
	TabControl(Window* parent, DWORD style);
	~TabControl();

	const wchar_t* getClass() override { return L"toadpad.tabcontrol"; }

	inline uint32_t GetItemCount() const { return mTabsInfo.count; }
	inline int SelectedIndex() const { return mTabsInfo.selectedIndex; }
	inline int HoveredIndex() const { return mTabsInfo.hoveredIndex; }
	
	void InsertItem(const std::wstring& label);
	void Goto(UINT index);
	bool RemoveItem(unsigned int index);
	bool SwitchTabs(UINT swap1, UINT swap2);

	UINT ItemsAreaSize();

	bool SetDisplayArea(Window* dsp_area);
	Window* GetDisplayArea() const { return _hDsp; };
	
	bool ResizeItems(int cx, int cy);
	RECT AvailableArea() const;
	int IndexFromPt(POINT& pt);
	
	bool RectFromIndex(int index, RECT& out, ResourceType type);
	
	bool TestForCursor(const POINT& position, const RECT& rectangle);
	inline RECT CreateRect(LONG x, LONG y, LONG w, LONG h) const { return RECT{ x, y, x + w, y + h }; }
};

#endif