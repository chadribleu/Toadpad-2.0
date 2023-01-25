#include "gui/tabcontrol.h"

// TODO: Safe drag area (= the number of pixels before the drag request is accepted)

// TODO: Text display correction (4 pixels around the close button) don't forget to handle the size when there's no close button
// Variable tabs size with a maximum / minium width
// Border display correction

// TODO: Move tabs
// TODO: Add Border Size
// TODO: Tooltip control that shows the complete name of the document and \n its path

TabControl::TabControl(Window* parent, DWORD style)
{
	Gdiplus::GdiplusStartup(&mGdiToken, &mGdiGsi, nullptr);
	
	mTabsInfo.count = 0;
	mTabsInfo.hoveredIndex = -1;
	mTabsInfo.prevHoveredIndex = -1;
	mTabsInfo.prevSelectedIndex = -1;
	mTabsInfo.selectedIndex = -1;
	mTabsInfo.trackMouse = FALSE;

	WNDCLASSEXW wcx{ 0 };
	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = Application::Instance();
	wcx.lpszClassName = getClass();
	wcx.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wcx.lpfnWndProc = s_TCproc;
	wcx.style = CS_GLOBALCLASS;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClassEx(&wcx)) {
		MessageBox(NULL, L"!REGISTERCLASSEX", getClass(), MB_ICONERROR);
	}
	// Load bitmap
	InitButtons();
	InitPalette(mPalette);
	create(L"", 0, 0, 0, 0, style, 0, parent, static_cast<HMENU>(NULL));
}

TabControl::~TabControl()
{
	for (int i = 0; i < mTabsInfo.count; ++i) delete[] mTabsInfo.item[i].pszLabel;
	CUnregisterClass();
	Gdiplus::GdiplusShutdown(mGdiToken);
}

bool TabControl::RectFromIndex(int index, RECT& out, ResourceType type)
{
	if (index < 0 || index >= mTabsInfo.count) return false;
	switch (type) {
		case ResourceType::Tab:
		{
			SetRect(&out, mTabsInfo.item[index].rcBounds.left, 
				mTabsInfo.item[index].rcBounds.top, 
				mTabsInfo.item[index].rcBounds.right, 
				mTabsInfo.item[index].rcBounds.bottom);
			return true;
		}
		case ResourceType::CloseButton:
		{
			RECT rcItem = mTabsInfo.item[index].rcBounds;
			UINT uImageWidth = mCloseButton.gfx[static_cast<int>(mCloseButton.state)]->GetWidth();
			UINT uImageHeight = mCloseButton.gfx[static_cast<int>(mCloseButton.state)]->GetHeight();
			long rcWidth = rcItem.right - rcItem.left;
			long rcHeight = rcItem.bottom - rcItem.top;
			out.left = ((rcItem.left + rcWidth) - uImageWidth) - 4;
			out.top = (rcHeight / 2) - (uImageHeight / 2);
			out.right = out.left + uImageWidth;
			out.bottom = out.top + uImageHeight;
			return true;
		}
		case ResourceType::AddButton:
		{
			RECT rcItem = mTabsInfo.item[mTabsInfo.count - 1].rcBounds;
			UINT uImageWidth = mAddButton.gfx[static_cast<int>(mAddButton.state)]->GetWidth();
			UINT uImageHeight = mAddButton.gfx[static_cast<int>(mAddButton.state)]->GetHeight();
			long rcWidth = rcItem.left + (rcItem.right - rcItem.left);
			long rcHeight = rcItem.bottom - rcItem.top;
			long left = rcWidth + 4;
			long top = (rcHeight / 2) - (uImageWidth / 2);
			SetRect(&out, left, top, left + uImageWidth, top + uImageHeight);
			return true;
		}
	}
	return false;
}

// TODO: Label spacing configuration (0 to 15)
void TabControl::DrawItem(ItemSkeleton& drawInfo, Gdiplus::Font const& font, Gdiplus::StringFormat const& format)
{
	// Create the required brushes and pens
	SolidBrush brushSelected(mPalette.selected);
	SolidBrush brushHovered(mPalette.hover);
	SolidBrush brushNormal(mPalette.normal);
	SolidBrush brushLabelSelected(mPalette.fkgnd_active);
	SolidBrush brushLabelHovered(mPalette.fkgnd_hover);
	SolidBrush brushLabelNormal(mPalette.fkgnd_inactive);
	Pen penBorders(mPalette.outline);
	Pen penAccentColor(mPalette.userAccentColor, 1.0F);

	drawInfo.graphics->FillRectangle((drawInfo.index == mTabsInfo.selectedIndex) ? &brushSelected
		: (drawInfo.index == mTabsInfo.hoveredIndex) ? &brushHovered : &brushNormal, drawInfo.bounds);

	// Borders
	Point vertices[] =
	{
		// delimiters
		{ drawInfo.bounds.GetLeft() - 1, drawInfo.bounds.GetTop() },
		{ drawInfo.bounds.GetRight(), drawInfo.bounds.GetTop() },
		
		{ drawInfo.bounds.GetRight() - 1, drawInfo.bounds.GetTop() },
		{ drawInfo.bounds.GetRight() - 1, drawInfo.bounds.GetBottom() },
		
		{ drawInfo.bounds.GetRight(), drawInfo.bounds.GetBottom() - 1 },
		{ drawInfo.bounds.GetLeft() - 1, drawInfo.bounds.GetBottom() - 1 }
	};
	drawInfo.graphics->DrawLines(&penBorders, &vertices[0], 6);
	// finally, draw the accent color
	if (drawInfo.index == mTabsInfo.selectedIndex) {
		Point safeArea[2]{
			{ drawInfo.bounds.GetLeft(), drawInfo.bounds.GetTop() },
			{ drawInfo.bounds.GetRight() - 2, drawInfo.bounds.GetTop() }
		};
		drawInfo.graphics->DrawLine(&penAccentColor, safeArea[0], safeArea[1]);
	}
	RectF labelPosition(drawInfo.bounds.X + 5, drawInfo.bounds.Y, drawInfo.bounds.Width, drawInfo.bounds.Height);
	if (GetWindowStyle(m_self) & CTCS_CLOSEABLE && (drawInfo.index == mTabsInfo.selectedIndex || drawInfo.index == mTabsInfo.hoveredIndex)) {
		DrawCloseButton(drawInfo);
		RECT rcCloseButton{ 0 };
		RectFromIndex(drawInfo.index, rcCloseButton, ResourceType::CloseButton);
		labelPosition.Width -= ((rcCloseButton.right - rcCloseButton.left));
	}
	else {
		labelPosition.Width -= 12;
	}
	drawInfo.graphics->DrawString(mTabsInfo.item[drawInfo.index].pszLabel, mTabsInfo.item[drawInfo.index].cSize, &font, labelPosition, &format,
		(drawInfo.index == mTabsInfo.selectedIndex) ? &brushLabelSelected 
		: (drawInfo.index == mTabsInfo.hoveredIndex) 
		? &brushLabelHovered
		: &brushLabelNormal);
}

void TabControl::DrawCloseButton(ItemSkeleton& drawInfo)
{
	// Get state
	Bitmap* bmp = mCloseButton.gfx[static_cast<int>(mCloseButton.state)];
	if (bmp == nullptr) return;
	UINT width = bmp->GetWidth(), height = bmp->GetHeight();
	RECT rcOut{ 0 };
	RectFromIndex(drawInfo.index, rcOut, ResourceType::CloseButton);
	RectF imgRect(rcOut.left, rcOut.top, (rcOut.right - rcOut.left), (rcOut.bottom - rcOut.top));
	drawInfo.graphics->DrawImage(bmp, imgRect, 0, 0, width, height, UnitPixel);
}

// TODO: Draw bitmap (it does not work)
void TabControl::DrawAddTabButton(HDC hdc)
{
	// Get state
	Bitmap* bmp = mAddButton.gfx[static_cast<int>(mAddButton.state)];
	if (bmp == nullptr) return;
	Graphics gfxAddButton(hdc);
	RECT rcAddButton{ 0 };
	RectFromIndex(mTabsInfo.count - 1, rcAddButton, ResourceType::AddButton);
	RectF imgRect(rcAddButton.left, rcAddButton.top, bmp->GetWidth(), bmp->GetHeight());
	gfxAddButton.DrawImage(bmp, imgRect);
}

Gdiplus::Color TabControl::Blend(Gdiplus::ARGB color, Gdiplus::ARGB background)
{
	Gdiplus::Color destination(background);
	Gdiplus::Color source(color);

	BYTE	srcA = source.GetA(),
			srcR = source.GetR(),
			srcG = source.GetG(),
			srcB = source.GetB();

	BYTE	destA = destination.GetA(),
			destR = destination.GetR(),
			destG = destination.GetG(),
			destB = destination.GetB();

	BYTE aComponent = srcA + (destA * (255 - srcA) / 255);
	BYTE rComponent = (srcR * srcA + destR * destA * (255 - srcA) / 255) / aComponent;
	BYTE gComponent = (srcG * srcA + destB * destA * (255 - srcA) / 255) / aComponent;
	BYTE bComponent = (srcB * srcA + destB * destA * (255 - srcA) / 255) / aComponent;

	return Gdiplus::Color(Gdiplus::Color::MakeARGB(aComponent, rComponent, gComponent, bComponent));
}

void TabControl::AdjustBrightness(Gdiplus::Color& source, float value)
{
	UINT
		r = source.GetR() + value,
		g = source.GetG() + value,
		b = source.GetB() + value;
	source.SetValue(Gdiplus::Color::MakeARGB(source.GetA(),
		r > 255 ? 255 : r, 
		g > 255 ? 255 : g, 
		b > 255 ? 255 : b));
}

// TODO: Enum = array index
// Bitmap index[4]
void TabControl::InitButtons()
{
	mCloseButton.gfx[static_cast<int>(CommButtonState::Clicked)] = new Bitmap(LR"(assets\gfx\close_button_clicked.png)");
	mCloseButton.gfx[static_cast<int>(CommButtonState::Hovered)] = new Bitmap(LR"(assets\gfx\close_button_hovered.png)");
	mCloseButton.gfx[static_cast<int>(CommButtonState::Normal)] = new Bitmap(LR"(assets\gfx\close_button_normal.png)");
	mCloseButton.gfx[static_cast<int>(CommButtonState::Disabled)] = nullptr;
	mCloseButton.state = CommButtonState::Normal;

	mAddButton.gfx[static_cast<int>(CommButtonState::Clicked)] = new Bitmap(LR"(assets\gfx\add_tab_btn_clicked.png)");
	mAddButton.gfx[static_cast<int>(CommButtonState::Hovered)] = new Bitmap(LR"(assets\gfx\add_tab_btn_hovered.png)");
	mAddButton.gfx[static_cast<int>(CommButtonState::Normal)] = new Bitmap(LR"(assets\gfx\add_tab_btn_normal.png)");
	mAddButton.gfx[static_cast<int>(CommButtonState::Disabled)] = nullptr;
	mAddButton.state = CommButtonState::Normal;
}

bool TabControl::ResizeItem(UINT index, int cx, int cy)
{
	if (index >= mTabsInfo.count) return false;
	if (cx > mTabsInfo.item[index].maximumWidth || cx <= 0) cx = mTabsInfo.item[index].maximumWidth;
	if (cy > mTabsInfo.maximumHeight || cx <= 0) cy = mTabsInfo.maximumHeight;
	mTabsInfo.item[index].rcBounds.right = cx + mTabsInfo.item[index].rcBounds.left;
	RECT updateRc{ mTabsInfo.item[index].rcBounds.left, mTabsInfo.item[index].rcBounds.top, 
		mTabsInfo.item[index].rcBounds.right, mTabsInfo.item[index].rcBounds.bottom };
	for (UINT i = index + 1; i < mTabsInfo.count; ++i) {
		mTabsInfo.item[i].rcBounds.left = mTabsInfo.item[i - 1].rcBounds.right;
		mTabsInfo.item[i].rcBounds.right = (mTabsInfo.item[i].rcBounds.right - (mTabsInfo.item[i]).rcBounds.left) 
			+ mTabsInfo.item[i].rcBounds.left;
		updateRc.right += mTabsInfo.item[i].rcBounds.right;
	}
	InvalidateRect(m_self, &updateRc, FALSE);
}

void TabControl::InsertItem(const std::wstring& label)
{
	// Add new item to the array
	mTabsInfo.item.push_back(Item{});
	// Intialize the new item
	const size_t strSize = label.size() + 1;
	mTabsInfo.item[mTabsInfo.count].cSize = strSize;
	mTabsInfo.item[mTabsInfo.count].iIndex = mTabsInfo.count;
	mTabsInfo.item[mTabsInfo.count].pszLabel = new wchar_t[strSize];
	wcscpy_s(mTabsInfo.item[mTabsInfo.count].pszLabel, mTabsInfo.item[mTabsInfo.count].cSize, label.data());
	// Redraw the necessary part
	RECT position{ 1, 0, mTabsInfo.item[mTabsInfo.count].maximumWidth, mTabsInfo.maximumHeight};
	if (mTabsInfo.count > 0) {
		const size_t prevIndex = mTabsInfo.count - 1;
		SetRect(&position, 
			mTabsInfo.item[prevIndex].rcBounds.right, 
			mTabsInfo.item[prevIndex].rcBounds.top,
			mTabsInfo.item[prevIndex].rcBounds.right + mTabsInfo.item[prevIndex].maximumWidth, 
			mTabsInfo.item[prevIndex].rcBounds.top + mTabsInfo.maximumHeight);
	}
	SetRect(&mTabsInfo.item[mTabsInfo.count].rcBounds, position.left, position.top, position.right, position.bottom);
	InvalidateRect(m_self, &mTabsInfo.item[mTabsInfo.count].rcBounds, FALSE);
	++mTabsInfo.count;
	RECT rcClient{ 0 }, rcAddBtn{ 0 };
	GetClientRect(m_self, &rcClient);
	RectFromIndex(mTabsInfo.count - 1, rcAddBtn, ResourceType::AddButton);
	ResizeItems((rcClient.right - (rcAddBtn.right - rcAddBtn.left) - 4) / mTabsInfo.count, mTabsInfo.maximumHeight);
}

void TabControl::Goto(UINT index) 
{ 
	if (index < mTabsInfo.count && index != mTabsInfo.selectedIndex) {
		mTabsInfo.prevSelectedIndex = mTabsInfo.selectedIndex;
		mTabsInfo.selectedIndex = index;
		RECT redrawArea1{ 0 }, redrawArea2{ 0 };
		RectFromIndex(mTabsInfo.selectedIndex, redrawArea1, ResourceType::Tab);
		RectFromIndex(mTabsInfo.prevSelectedIndex, redrawArea2, ResourceType::Tab);
		InvalidateRect(m_self, &redrawArea1, FALSE);
		InvalidateRect(m_self, &redrawArea2, FALSE);
	}
}

bool TabControl::RemoveItem(unsigned int index)
{
	if (index >= mTabsInfo.count) return false;
	// Move tabs
	const size_t count = mTabsInfo.count;
	// Todo move tabs to the left
	for (int i = index + 1; i < count; ++i) {
		OffsetRect(&mTabsInfo.item[i].rcBounds, 
			(mTabsInfo.item[i - 1].rcBounds.right - mTabsInfo.item[i - 1].rcBounds.left) * (-1), 0);
		--mTabsInfo.item[i].iIndex;
	}
	// Invalidate tabs from the deleted tab to the last tab
	RECT rcClient{ 0 };
	GetClientRect(m_self, &rcClient);
	RECT updateRc{ mTabsInfo.item[index].rcBounds.left, 0, rcClient.right, mTabsInfo.maximumHeight};
	InvalidateRect(m_self, &updateRc, FALSE);
	// Delete the current tab's informations
	delete[] mTabsInfo.item[index].pszLabel;
	mTabsInfo.item.erase((mTabsInfo.item.begin()) + index);
	--mTabsInfo.count;
	if (mTabsInfo.count > 1) {
		RECT rcAddBtn{ 0 };
		RectFromIndex(mTabsInfo.count - 1, rcAddBtn, ResourceType::AddButton);
		ResizeItems((rcClient.right - (rcAddBtn.right - rcAddBtn.left) - 4) / mTabsInfo.count, mTabsInfo.maximumHeight);
	}
	return true;
}

// TODO: Also get the Y position
UINT TabControl::ItemsAreaSize()
{
	UINT areaWidth = 0;
	for (int i = 0; i < mTabsInfo.count; ++i) {
		areaWidth += mTabsInfo.item[i].rcBounds.right - mTabsInfo.item[i].rcBounds.left;
	}
	return areaWidth;
}

bool TabControl::SetDisplayArea(Window* dsp_area)
{
	_hDsp = dsp_area;
	HWND handle = _hDsp->getHandle();
	UINT_PTR style = GetWindowStyle(handle);
	UINT_PTR exStyle = GetWindowExStyle(handle);
	if (style & WS_CAPTION) style &= ~WS_CAPTION;
	return _hDsp->repaint(style, exStyle, this->getHandle());
}

bool TabControl::ResizeItems(int cx, int cy)
{
	for (int i = 0; i < mTabsInfo.count; ++i) {
		if (!ResizeItem(i, cx, cy)) return false;
	}
	return true;
}

RECT TabControl::AvailableArea() const
{
	RECT rcArea{ 0 };
	GetClientRect(m_self, &rcArea);
	rcArea.left = 1;
	rcArea.top += mTabsInfo.maximumHeight;
	rcArea.right -= 1;
	rcArea.bottom -= 1;
	return rcArea;
}

int TabControl::IndexFromPt(POINT& pt)
{
	for (int i = 0; i < mTabsInfo.count; ++i) {
		if ((pt.x >= mTabsInfo.item[i].rcBounds.left && pt.x <= mTabsInfo.item[i].rcBounds.right)
			&& (pt.y >= mTabsInfo.item[i].rcBounds.top && pt.y <= mTabsInfo.item[i].rcBounds.bottom)) {
			return i;
		}
	}
	return -1;
}

bool TabControl::TestForCursor(const POINT& position, const RECT& rectangle)
{
	return ((position.x >= rectangle.left && position.x <= rectangle.right) && (position.y >= rectangle.top && position.y <= rectangle.bottom));
}

BOOL TabControl::CUnregisterClass()
{
	return UnregisterClassW(getClass(), Application::Instance());
}

// Todo: Window->SetPalette(ColorPalette& palette); mPalette member
// Todo: bool enable depth effect
void TabControl::InitPalette(ColorPalette& palette)
{
	// Sys color
	DWORD colorWindow = GetSysColor(COLOR_WINDOW);
	int rColorWindow = GetRValue(colorWindow);
	int gColorWindow = GetGValue(colorWindow);
	int bColorWindow = GetBValue(colorWindow);

	palette.bkgnd = Color::MakeARGB(255, 240, 240, 240);
	
	// Foreground fgnd
	palette.fkgnd_active = Color::MakeARGB(255, 59, 59, 59);
	palette.fkgnd_inactive = Color::MakeARGB(255, 59, 59, 59);
	palette.fkgnd_hover = Color::MakeARGB(255, 59, 59, 59);
	
	palette.outline = Color::MakeARGB(255, 227, 227, 227); // border

	// Tab state
	palette.normal = palette.bkgnd;
	palette.hover = Color::MakeARGB(255, 250, 250, 250);
	palette.selected = Color::MakeARGB(255, 245, 245, 245);
	palette.disabled = Color::MakeARGB(255, 0, 0, 0); // unused for the moment

	palette.userAccentColor = Color::MakeARGB(255, 0, 0, 255);
}

LRESULT TabControl::s_TCproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (hWnd == NULL) {
		return 0;
	}
	TabControl* thisPtr = reinterpret_cast<TabControl*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	if (uMsg == WM_NCCREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		thisPtr = reinterpret_cast<TabControl*>(cs->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisPtr));
		thisPtr->m_self = hWnd;
	}
	else {
		thisPtr = reinterpret_cast<TabControl*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}
	if (thisPtr) {
		return thisPtr->HandleMessage(uMsg, wParam, lParam);
	}
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool TabControl::SwitchTabs(UINT swap1, UINT swap2)
{
	if (swap1 >= mTabsInfo.count || swap2 >= mTabsInfo.count) return false;
	std::swap(mTabsInfo.item[swap1].cSize, mTabsInfo.item[swap2].cSize);
	std::swap(mTabsInfo.item[swap1].pszLabel, mTabsInfo.item[swap2].pszLabel);
	std::swap(mTabsInfo.item[swap1].maximumWidth, mTabsInfo.item[swap2].maximumWidth);
	// Resize items
	RECT rcTempA = mTabsInfo.item[swap1].rcBounds, rcTempB = mTabsInfo.item[swap2].rcBounds;
	ResizeItem(swap1, rcTempB.right - rcTempB.left, mTabsInfo.maximumHeight);
	ResizeItem(swap2, rcTempA.right - rcTempA.left, mTabsInfo.maximumHeight);
	InvalidateRect(m_self, &rcTempA, FALSE);
	InvalidateRect(m_self, &rcTempB, FALSE);
}

LRESULT TabControl::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (mTabsInfo.trackMouse == FALSE) {
			TRACKMOUSEEVENT tme{ 0 };
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_self;
			mTabsInfo.trackMouse = TrackMouseEvent(&tme);
		}
		// Detect whether a tab has been hovered or not.
		mTabsInfo.prevHoveredIndex = mTabsInfo.hoveredIndex;
		POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		mTabsInfo.hoveredIndex = IndexFromPt(pt);
		if (mTabsInfo.hoveredIndex != mTabsInfo.prevHoveredIndex) {
			RECT prevRc{}, currRc{};
			if (mTabsInfo.prevHoveredIndex != mTabsInfo.selectedIndex) {
				RectFromIndex(mTabsInfo.prevHoveredIndex, prevRc, ResourceType::Tab);
				InvalidateRect(m_self, &prevRc, FALSE);
			}
			if (mTabsInfo.hoveredIndex != mTabsInfo.selectedIndex) {
				RectFromIndex(mTabsInfo.hoveredIndex, currRc, ResourceType::Tab);
				InvalidateRect(m_self, &currRc, FALSE);
			}
		}

		// TODO 20/08/22:
		//     - DragRequest::Pending -> if mouseX > delta + 5 -> DragRequest::Accept; isDragging = true;
		////////////////////////// - DRAG ITEM SECTION - ///////////////////////////////////////////////
		UINT keyDown = (UINT)wParam;
		if (mTabsInfo.draggedItem.dragRequest == DragRequest::Ask) mTabsInfo.draggedItem.dragRequest = DragRequest::Pending;
		
		if (mTabsInfo.draggedItem.dragRequest == DragRequest::Pending && keyDown == MK_LBUTTON) {
			if (pt.x > (mTabsInfo.draggedItem.delta.x + 50) || pt.x < (mTabsInfo.draggedItem.delta.x) - 50) { // todo: Move delay
				mTabsInfo.draggedItem.dragRequest = DragRequest::Accepted;
				mTabsInfo.isItemDragged = true;
			}
		}
		if (mTabsInfo.isItemDragged && keyDown == MK_LBUTTON) {
			mTabsInfo.draggedItem.previousRect = mTabsInfo.draggedItem.posAtScreen; // store previous rect, then update
			
			HDC dc = GetDC(m_self);
			HDC hdcMem = CreateCompatibleDC(dc);
			HGDIOBJ orig = SelectObject(hdcMem, mTabsInfo.draggedItem.cache); // restore and delete

			mTabsInfo.draggedItem.posAtScreen.left = GET_X_LPARAM(lParam) - mTabsInfo.draggedItem.delta.x;
			mTabsInfo.draggedItem.posAtScreen.top = GET_Y_LPARAM(lParam);

			POINT ptA = { mTabsInfo.draggedItem.posAtScreen.left + mTabsInfo.draggedItem.delta.x, mTabsInfo.maximumHeight }, 
				ptB = { mTabsInfo.draggedItem.posAtScreen.right, mTabsInfo.maximumHeight };
			RECT client;
			GetClientRect(m_self, &client);

			InvalidateRect(m_self, &mTabsInfo.draggedItem.previousRect, FALSE);
			UpdateWindow(m_self);

			// draw using the cached bitmap
			BitBlt(dc, mTabsInfo.draggedItem.posAtScreen.left,
				mTabsInfo.item[mTabsInfo.draggedItem.which].rcBounds.top,
				mTabsInfo.draggedItem.posAtScreen.left + mTabsInfo.draggedItem.posAtScreen.right,
				mTabsInfo.draggedItem.posAtScreen.top + mTabsInfo.draggedItem.posAtScreen.bottom,
				hdcMem, 0, 0, SRCCOPY);

			ReleaseDC(m_self, dc);
			SelectObject(hdcMem, orig);
			DeleteDC(hdcMem);
		}
		////////////////////////////////////////////////////////////////////////////////////////////

		if (GetWindowStyle(m_self) & CTCS_CLOSEABLE) {
			RECT rcItem{}, out{}, prevOut{}, temp{};
			RectFromIndex(mTabsInfo.hoveredIndex, rcItem, ResourceType::Tab);
			CommButtonState prevBtnState = mCloseButton.state;
			if (mTabsInfo.hoveredIndex != -1) {
				RectFromIndex(mTabsInfo.hoveredIndex, out, ResourceType::CloseButton);
			}
			if (mTabsInfo.prevHoveredIndex != -1) {
				RectFromIndex(mTabsInfo.prevHoveredIndex, prevOut, ResourceType::CloseButton);
			}
			if (TestForCursor(pt, out)) {
				mCloseButton.state = (keyDown == MK_LBUTTON ? 
					CommButtonState::Clicked : CommButtonState::Hovered);
			}
			else {
				mCloseButton.state = CommButtonState::Normal;
			}
			if (mCloseButton.state != prevBtnState) {
				InvalidateRect(m_self, &out, FALSE);
				InvalidateRect(m_self, &prevOut, FALSE);
			}
		}
		RECT rcAddBtn{ 0 };
		RectFromIndex(mTabsInfo.count - 1, rcAddBtn, ResourceType::AddButton);
		CommButtonState prevBtnState = mAddButton.state;
		if (TestForCursor(pt, rcAddBtn)) {
			mAddButton.state = (keyDown == MK_LBUTTON ? 
				CommButtonState::Clicked : CommButtonState::Hovered);
		}
		else {
			mAddButton.state = CommButtonState::Normal;
		}
		if (prevBtnState != mAddButton.state) {
			InvalidateRect(m_self, &rcAddBtn, FALSE);
		}
		return 0;
	}

	case WM_MOUSELEAVE: // Invalidate the rectangle that was hovered
	{
		if (mTabsInfo.hoveredIndex != mTabsInfo.selectedIndex) {
			RECT hoveredRc{ 0 };
			RectFromIndex(mTabsInfo.hoveredIndex, hoveredRc, ResourceType::Tab);
			InvalidateRect(m_self, &hoveredRc, FALSE);
			mTabsInfo.hoveredIndex = -1;
		}
		mTabsInfo.trackMouse = FALSE;
		return TRUE;
	}

	case WM_LBUTTONDOWN:
	{
		POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		RECT rcCloseBtn{ 0 };
		RECT rcAddBtn{ 0 };
		RectFromIndex(mTabsInfo.hoveredIndex, rcCloseBtn, ResourceType::CloseButton);
		RectFromIndex(mTabsInfo.count - 1, rcAddBtn, ResourceType::AddButton);
		if (TestForCursor(pt, rcCloseBtn)) {
			mCloseButton.state = CommButtonState::Clicked;
			InvalidateRect(m_self, &rcCloseBtn, FALSE);
		}
		else if (TestForCursor(pt, rcAddBtn)) {
			mAddButton.state = CommButtonState::Clicked;
			InvalidateRect(m_self, &rcAddBtn, FALSE);
		}
		else {
			int index = IndexFromPt(pt);

			// DRAG AREA ///////////////////////////////////////////////////////////////////////////////////
			mTabsInfo.draggedItem.dragRequest = DragRequest::Ask;
			mTabsInfo.draggedItem.which = index;
			HDC hdc = GetDC(m_self);
			HDC memDC = CreateCompatibleDC(hdc);
			RECT rcDraggedItem{ 0 };
			RectFromIndex(index, rcDraggedItem, ResourceType::Tab);
			long width = mTabsInfo.item[index].rcBounds.right - mTabsInfo.item[index].rcBounds.left, 
				height = mTabsInfo.item[index].rcBounds.bottom - mTabsInfo.item[index].rcBounds.top;
			HBITMAP cache = CreateCompatibleBitmap(hdc, width, height);
			HGDIOBJ originalState = SelectObject(memDC, cache);
			// Draw on the bitmap
			BitBlt(memDC, 0, 0, width, height, hdc, mTabsInfo.item[index].rcBounds.left, mTabsInfo.item[index].rcBounds.top, SRCCOPY);
			// fill struct
			mTabsInfo.draggedItem.posAtScreen = rcDraggedItem;
			mTabsInfo.draggedItem.cache = cache;
			mTabsInfo.draggedItem.delta = { pt.x - mTabsInfo.draggedItem.posAtScreen.left, pt.y - mTabsInfo.draggedItem.posAtScreen.top };
			SelectObject(memDC, originalState);
			ReleaseDC(m_self, hdc);
			DeleteDC(memDC);
			//////////////////////////////////////////////////////////////////////////////////////////////

			if (index != mTabsInfo.selectedIndex && index != -1) {
				mTabsInfo.prevSelectedIndex = mTabsInfo.selectedIndex;
				mTabsInfo.selectedIndex = index;
				RECT prevRc{}, currRc{};
				RectFromIndex(mTabsInfo.prevSelectedIndex, prevRc, ResourceType::Tab);
				RectFromIndex(mTabsInfo.selectedIndex, currRc, ResourceType::Tab);
				InvalidateRect(m_self, &prevRc, FALSE);
				InvalidateRect(m_self, &currRc, FALSE);
				UpdateWindow(m_self);
				NMHDR nmhdr{};
				nmhdr.hwndFrom = m_self;
				nmhdr.idFrom = GetDlgCtrlID(m_self);
				nmhdr.code = CTCN_SELCHANGE;
				SendMessageW(this->m_parent, WM_NOTIFY, static_cast<WPARAM>(mTabsInfo.prevSelectedIndex), reinterpret_cast<LPARAM>(&nmhdr));
			}
		}
		return 0;
	}

	case WM_LBUTTONUP:
	{
		mTabsInfo.draggedItem.dragRequest = DragRequest::None;
		// do cleanup and cancel dragging
		if (mTabsInfo.isItemDragged) {
			mTabsInfo.isItemDragged = false;
			if (mTabsInfo.hoveredIndex != -1 && mTabsInfo.hoveredIndex != mTabsInfo.draggedItem.which) {
				// Update tabs
				SwitchTabs(mTabsInfo.draggedItem.which, mTabsInfo.hoveredIndex);
				NMHDR nmh;
				nmh.hwndFrom = m_self;
				nmh.idFrom = GetDlgCtrlID(m_self);
				nmh.code = CTCN_TABSWITCH;
				SendMessage(BaseParent(), WM_NOTIFY, MAKEWPARAM(mTabsInfo.draggedItem.which, mTabsInfo.hoveredIndex), 
					reinterpret_cast<LPARAM>(&nmh));
			}
			mTabsInfo.draggedItem.which = -1;
			if (FALSE == DeleteObject(mTabsInfo.draggedItem.cache)) MessageBox(m_self,
				L"Failed to delete the previous dragged item's cache,\ncontact the developer for more informations.",
				APPNAME, MB_ICONERROR);
			mTabsInfo.draggedItem.delta = { -1 };
			mTabsInfo.draggedItem.posAtScreen = { -1 };
		}
		
		// Update the state of buttons and tabs
		POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		if (GetWindowStyle(m_self) & CTCS_CLOSEABLE) { // or CTCS_USERDEFINED
			RECT rcBtn{ 0 };
			RectFromIndex(mTabsInfo.hoveredIndex, rcBtn, ResourceType::CloseButton);
			if (TestForCursor(pt, rcBtn)) {
				mCloseButton.state = CommButtonState::Normal;
				NMHDR nmhdr{};
				nmhdr.hwndFrom = m_self;
				nmhdr.idFrom = GetDlgCtrlID(m_self);
				nmhdr.code = CTCN_CLOSEBUTTONPRESSED;
				SendMessageW(BaseParent(), WM_NOTIFY, static_cast<WPARAM>(mTabsInfo.hoveredIndex), reinterpret_cast<LPARAM>(&nmhdr));
			}
		}
		const size_t count = mTabsInfo.count - 1;
		RECT rcAddBtn{ 0 };
		RectFromIndex(count, rcAddBtn, ResourceType::AddButton);
		if (TestForCursor(pt, rcAddBtn)) {
			mAddButton.state = CommButtonState::Normal;
			NMHDR nmhdr{ 0 };
			nmhdr.hwndFrom = m_self;
			nmhdr.idFrom = GetDlgCtrlID(m_self);
			nmhdr.code = CTCN_ADDBUTTONPRESSED;
			SendMessage(BaseParent(), WM_NOTIFY, static_cast<WPARAM>(0), reinterpret_cast<LPARAM>(&nmhdr));
		}
		return 0;
	}

	case WM_ERASEBKGND:
		return 1;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(m_self, &ps);
		RECT rcClient{ 0 };
		GetClientRect(m_self, &rcClient);

		if (bDoubleBuffering) {
			Rect rcPaintingArea(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, 
				ps.rcPaint.bottom - ps.rcPaint.top);
			Rect rcClientArea(rcClient.left, rcClient.top, rcClient.right - rcClient.left, 
				rcClient.bottom - rcClient.top);
			HDC hdcBackbuff = CreateCompatibleDC(ps.hdc);
			HBITMAP hbmBuff = CreateCompatibleBitmap(ps.hdc, rcClientArea.Width, rcClientArea.Height);
			
			HGDIOBJ originalObject = SelectObject(hdcBackbuff, hbmBuff);
			
			Graphics *backbuffer = Graphics::FromHDC(hdcBackbuff);
			
			// resources
			Pen penBackground(mPalette.outline);
			SolidBrush brBackground(mPalette.bkgnd);
			FontFamily family(L"Segoe UI Semibold");
			Font fnt(&family, 10, NULL, UnitPoint);
			StringFormat format;
			format.SetAlignment(StringAlignmentNear);
			format.SetTrimming(StringTrimmingEllipsisCharacter);
			format.SetFormatFlags(StringFormatFlagsNoWrap);
			format.SetLineAlignment(StringAlignmentCenter);

			// draw
			backbuffer->FillRectangle(&brBackground, rcClientArea);
			backbuffer->DrawRectangle(&penBackground, rcClientArea);
			for (int i = 0; i < mTabsInfo.count; ++i) {
				ItemSkeleton drawInfo{ i, backbuffer, Rect(mTabsInfo.item[i].rcBounds.left, mTabsInfo.item[i].rcBounds.top, 
					mTabsInfo.item[i].rcBounds.right - mTabsInfo.item[i].rcBounds.left, mTabsInfo.item[i].rcBounds.bottom - mTabsInfo.item[i].rcBounds.top) };
				if (drawInfo.bounds.IntersectsWith(rcPaintingArea)) {
					if (!(i == mTabsInfo.draggedItem.which && mTabsInfo.isItemDragged)) DrawItem(drawInfo, fnt, format);
				}
			}
			DrawAddTabButton(hdcBackbuff);
			
			// copy
			BitBlt(ps.hdc, 0, 0, rcClientArea.Width, rcClientArea.Height, hdcBackbuff, 0, 0, SRCCOPY);
			
			// do cleanup
			SelectObject(hdcBackbuff, originalObject);
			if (FALSE == DeleteDC(hdcBackbuff)) MessageBox(m_self, 
				L"Failed to delete the Backbuffer's DC,\ncontact the developer for more information.", L"Memory Leak", MB_ICONERROR);
			if (FALSE == DeleteObject(hbmBuff)) MessageBox(m_self, 
				L"Failed to delete the memory bitmap's buffer,\ncontact the developer for more information.", L"Memory Leak", MB_ICONERROR);
		}
		EndPaint(m_self, &ps);
		return TRUE;
	}

	case WM_COMMAND:
		SendMessageW(BaseParent(), CTCM_ONCOMMAND, wParam, lParam);
		break;

	case WM_NCCALCSIZE:
	{
		if (static_cast<BOOL>(wParam == TRUE)) {
			NCCALCSIZE_PARAMS* nccp = reinterpret_cast<LPNCCALCSIZE_PARAMS>(lParam);
			RECT rcAddBtn{ 0 };
			RectFromIndex(mTabsInfo.count - 1, rcAddBtn, ResourceType::AddButton);
			ResizeItems((nccp->rgrc[0].right - (rcAddBtn.right - rcAddBtn.left) - 4) / mTabsInfo.count, mTabsInfo.maximumHeight);
		}
	}
	break;

	case WM_SIZE:
	{
		if (_hDsp != nullptr) {
			RECT clientRect = AvailableArea();
			_hDsp->setGeometry(PosInfo{ clientRect.left, clientRect.top, 
				clientRect.right - clientRect.left, clientRect.bottom - clientRect.top });
		}
		return 0;
	}

	default:
		break;
	}
	return DefWindowProcW(m_self, uMsg, wParam, lParam);
}
