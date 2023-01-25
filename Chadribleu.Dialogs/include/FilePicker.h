#ifndef FILEPICKER_H
#define FILEPICKER_H

#include "pch.h"

// -- FilePicker lib update:
//    - Multiselection
//    - Append extension to the file
//    - Exceptions (only char for the moment)

#define SIZE_FILTERS_BUFF 1024

class CHADRIBLEUDIALOGS_API FilePicker {
public:
	enum class Mode {
		SAVE,
		OPEN
	};

	FilePicker(HWND owner, FilePicker::Mode mode, const std::wstring& caption = L"");
	~FilePicker();

	inline const std::wstring GetResult() { return std::wstring(m_ofn.lpstrFile); }
	bool ShowDialog(const std::wstring& filename);
	void SetFilters(const std::wstring& filters, int defaultIndex = 1);
	void SetDefaultExt(const wchar_t ext[3]);

private:
	OPENFILENAME m_ofn;
	FilePicker::Mode m_dlgStyle;
	wchar_t* m_defExt;
	wchar_t* m_fileBuffer;
	wchar_t* m_filtersBuffer;
};

#endif