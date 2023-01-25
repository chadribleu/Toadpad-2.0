#include "pch.h"
#include "FilePicker.h"

FilePicker::FilePicker(HWND owner, FilePicker::Mode mode, const std::wstring& caption) : m_dlgStyle(mode), m_ofn{ 0 } 
{
	// Initialize variables
	const wchar_t* titleBarText = caption.data();
	m_fileBuffer = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, sizeof(wchar_t) * MAX_PATH);
	m_filtersBuffer = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, sizeof(wchar_t) * SIZE_FILTERS_BUFF);
	m_defExt = (wchar_t*)HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, sizeof(wchar_t) * 3);
	// Initialize structure
	m_ofn.lStructSize = sizeof(OPENFILENAMEW);
	m_ofn.hwndOwner = owner;
	m_ofn.hInstance = GetModuleHandleW(NULL);
	m_ofn.lpstrTitle = titleBarText ? titleBarText : nullptr;
	m_ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
}

bool FilePicker::ShowDialog(const std::wstring& filename) 
{
	size_t maxStrSize = filename.length();
	if (filename[0] && maxStrSize < (MAX_PATH - 1)) {
		if (maxStrSize > MAX_PATH) --maxStrSize;
		if (int ret = wmemcpy_s(m_fileBuffer, MAX_PATH, filename.c_str(), maxStrSize); ret != 0) {
			throw std::runtime_error("Error at wmemcpy_s. contact the developer to fix this bug.");
		}
		m_fileBuffer[maxStrSize] = L'\0';
	}
	m_ofn.nMaxFile = MAX_PATH;
	m_ofn.lpstrFile = m_fileBuffer;
	BOOL result = FALSE;
	switch (m_dlgStyle) {
	case FilePicker::Mode::SAVE:
		result = GetSaveFileNameW(&m_ofn);
		break;
	case FilePicker::Mode::OPEN:
		result = GetOpenFileNameW(&m_ofn);
		break;
	}
	// Find the last four characters and compose the final path if there's no extension
	if (m_ofn.nFileExtension == 0) {
		size_t extensionOffset = m_ofn.nFileOffset;
		if (extensionOffset >= MAX_PATH) extensionOffset = MAX_PATH - 4;
		const int filtersSize = lstrlen(m_ofn.lpstrFilter);
		int endStringCounter = 0;
		int selExtOffset;
		// Text files (*.txt)\0.txt\0\0
		for (selExtOffset = 0; selExtOffset < filtersSize; ++selExtOffset) {
			if (m_ofn.lpstrFilter[selExtOffset] == L'\0') {
				endStringCounter++;
			}
		}
		// Get string size after appending the extension
		const size_t filenameSize = lstrlen(m_fileBuffer);
		wchar_t buffer[5];
		swprintf_s(buffer, 5, L".%.*s", 3, &m_ofn.lpstrFilter[selExtOffset +3]);
		wmemcpy_s(&m_fileBuffer[filenameSize], MAX_PATH - 5, &buffer[0], 5);
	}
	return static_cast<bool>(result);
}

void FilePicker::SetFilters(const std::wstring& filters, int defaultIndex)
{
	size_t stringSize = filters.length();
	size_t counter = 0;
	if (stringSize > SIZE_FILTERS_BUFF) --stringSize;
	for (;counter < stringSize; ++counter) {
		if (filters[counter] == L'|') m_filtersBuffer[counter] = L'\0';
		else m_filtersBuffer[counter] = filters[counter];
	}
	m_filtersBuffer[stringSize] = L'\0';
	m_filtersBuffer[stringSize + 1] = L'\0';
	m_ofn.nFilterIndex = defaultIndex;
	m_ofn.lpstrFilter = m_filtersBuffer;
}

void FilePicker::SetDefaultExt(const wchar_t ext[3])
{
	wmemcpy_s(m_defExt, 3, ext, 3);
	m_ofn.lpstrDefExt = m_defExt;
}

FilePicker::~FilePicker() 
{
	HeapFree(GetProcessHeap(), NULL, m_fileBuffer);
	HeapFree(GetProcessHeap(), NULL, m_filtersBuffer);
	HeapFree(GetProcessHeap(), NULL, m_defExt);
}