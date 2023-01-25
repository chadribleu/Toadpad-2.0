#include "utils/clipboard.h"

Clipboard::Clipboard() {}

Clipboard::Clipboard(const Window* parent) {
	m_parent = parent->getHandle();
}

Clipboard::~Clipboard() {
	CloseClipboard();
}

bool Clipboard::Open(bool delayedRendering) {
	if (OpenClipboard(!delayedRendering ? m_parent : NULL) == TRUE) {
		return true;
	}
	return false;
}

bool Clipboard::Close() {
	if (CloseClipboard() == TRUE) {
		return true;
	}
	return false;
}

void Clipboard::SetText(const wchar_t* text) {
	unsigned size = Utils::StringSize(text);
	HGLOBAL hMemory = GlobalAlloc(GHND, size * sizeof(wchar_t));
	wchar_t* lpText = (wchar_t*)GlobalLock(hMemory);
	
	if (lpText != NULL) {
		wchar_t* tmpPtr = Utils::StringCopy(lpText, text, size);
		if (tmpPtr) {
			GlobalUnlock(hMemory);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hMemory);
		}
	}
}

std::wstring Clipboard::GetText() {
	HANDLE hMem = NULL;
	std::wstring text;
	wchar_t* buff = NULL;

	if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		hMem = GetClipboardData(CF_UNICODETEXT);
		if (hMem) {
			buff = (wchar_t*)GlobalLock(hMem);
			GlobalUnlock(hMem);
			text = buff;
		}
	}
	return text;
}

bool Clipboard::Clear() {
	if (EmptyClipboard() == TRUE) {
		return true;
	}
	return false;
}

std::vector<UINT> Clipboard::EnumAvailableFormats() {
	UINT result = 0;
	std::vector<UINT> fTable;
	while (result != ERROR_SUCCESS) {
		result = GetLastError();
		fTable.push_back(EnumClipboardFormats(NULL));
	}
	return fTable;
}