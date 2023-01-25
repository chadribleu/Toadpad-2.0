#include "gui/edit.h"

LRESULT CALLBACK Edit::s_EditSubclassProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam, [[maybe_unused]]UINT_PTR identifier, DWORD_PTR refData)
{
	if (handle == nullptr)
		return NULL;

	auto pEdit = std::bit_cast<Edit*>(refData);
	if (pEdit) {
		return pEdit->HandleMessage(msg, wParam, lParam);
	}
	else {
		return DefSubclassProc(handle, msg, wParam, lParam);
	}
}

LRESULT Edit::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case CWM_SUBCLASSED:
		{
			auto dwIntStyle = static_cast<DWORD>(this->Notify(EM_GETEXTENDEDSTYLE, nullptr, nullptr));
			this->Notify(EM_SETEXTENDEDSTYLE, dwIntStyle, ES_EX_ZOOMABLE);
			ZoomInfos zoomInfos = this->GetZoom();
			this->Notify(EM_SETZOOM, zoomInfos.numerator, zoomInfos.denominator);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			if (LOWORD(wParam) == MK_CONTROL) {
				static_cast<short>(HIWORD(wParam)) >= wheel_delta ? SetZoom(zoom_range) : SetZoom(-zoom_range);
			}
			else {
				DefSubclassProc(m_self, msg, wParam, lParam);
			}
			return 0;
		}

		case WM_LBUTTONDOWN:
		case WM_KEYDOWN:
		case WM_CHAR:
		{
			DefSubclassProc(m_self, msg, wParam, lParam);
			POINT caretPos{ 0 };
			GetCaretPos(&caretPos);
			LRESULT notifyValue = this->Notify(EM_CHARFROMPOS, 0, MAKELPARAM(caretPos.x, caretPos.y));
			m_caretPos.line = HIWORD(notifyValue) + 1;
			auto lineIdx = static_cast<int>(this->Notify<int, LPARAM>(EM_LINEINDEX, m_caretPos.line - 1));
			m_caretPos.column = (LOWORD(notifyValue) - lineIdx) + 1;
			SendMessage(this->BaseParent(), CEM_GETLINEINFO, MAKEWPARAM(m_caretPos.line, m_caretPos.column), 0);
			return 0;
		}
		default:
			return DefSubclassProc(m_self, msg, wParam, lParam);
	}
}

Edit::Edit(Window* parent, int width, int height, const wchar_t* text, DWORD style, DWORD exStyle, const int identifier)
{
	this->create(text, 0, 0, width, height, style, exStyle, parent, reinterpret_cast<HMENU>(identifier));
	this->Subclass(Edit::s_EditSubclassProc, 50);
}

Edit::~Edit()
{
	return;
}

bool Edit::enableWordWrap(bool wordwrap)
{
	if ((m_style & ES_MULTILINE) == NULL)
		return false;

	if (HLOCAL tmpMem = LocalAlloc(GHND, sizeof(wchar_t));tmpMem == nullptr) {
		return false;
	}
	else {
		auto currBuff = std::bit_cast<HLOCAL>(this->Notify(EM_GETHANDLE, NULL, NULL));
		this->swapMem(tmpMem);

		auto style = static_cast<DWORD>(GetWindowLongPtrW(m_self, GWL_STYLE));
		auto exStyle = static_cast<DWORD>(GetWindowLongPtrW(m_self, GWL_EXSTYLE));

		if ((!wordwrap) && ((style & WS_HSCROLL) == NULL)) {
			m_wordwrap = false;
			style |= WS_HSCROLL;
		}
		else if (wordwrap && (style & WS_HSCROLL) == WS_HSCROLL) {
			m_wordwrap = true;
			style &= ~WS_HSCROLL;
		}
		if (!this->repaint(style, exStyle, m_parent)) {
			return false;
		}
		this->swapMem(currBuff);
		LocalFree(tmpMem);
	}
	return true;
}

// Note: SwapMem does not clean the old local memory handle
void Edit::swapMem(HLOCAL buff)
{
	this->Notify<HLOCAL, int>(EM_SETHANDLE, buff, NULL);
}

void Edit::ReplaceSel(const std::wstring& string, bool canUndo)
{
	this->Notify<BOOL, const wchar_t*>(EM_REPLACESEL, canUndo, string.c_str());
}

void Edit::undoText()
{
	this->Notify<int, int>(EM_UNDO, NULL, NULL);
}

void Edit::SetZoom(int percentage)
{
	this->Notify(EM_GETZOOM, &m_zInfos.numerator, &m_zInfos.denominator); 
	if (percentage > 0)
		m_zInfos.numerator = CHOOSEMIN(max_zoom, (m_zInfos.numerator + percentage));
	else if (percentage < 0)
		m_zInfos.numerator = CHOOSEMAX(min_zoom, m_zInfos.numerator + percentage);
	else
		m_zInfos.numerator = zoom_default_val;
	this->Notify(EM_SETZOOM, m_zInfos.numerator, zoom_default_val);
	SendMessage(GetParent(m_parent), CEM_ZOOMCHANGED, static_cast<WPARAM>(percentage), static_cast<LPARAM>(m_zInfos.numerator));
}

Edit::ZoomInfos Edit::GetZoom() const
{
	return m_zInfos;
}

Edit::SelectionRange Edit::getSelection()
{
	SelectionRange sr{ 0, 0 };
	this->Notify(EM_GETSEL, &sr.begin, &sr.end);
	return sr;
}

Edit::CaretPosition Edit::GetLineInfo() const
{
	return m_caretPos;
}

void Edit::SetSelection(SelectionRange const& range)
{
	this->Notify(EM_SETSEL, range.begin, range.end);
}

std::wstring Edit::GetSelText()
{
	SelectionRange usrSelection = this->getSelection();
	const int size = usrSelection.end - usrSelection.begin + 1;
	const auto thisText = this->getText();
	auto selStr = new wchar_t[size];
	wcsncpy_s(selStr, size, &thisText[usrSelection.begin], usrSelection.end);
	std::wstring out(selStr);
	delete[] selStr;
	return out;
}