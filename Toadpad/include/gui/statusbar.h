#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include "common.h"
#include "window.h"

#define MAXPARTS 256

class StatusBar : public Window
{
private:
	int m_nParts;

public:
	StatusBar(Window* parent, DWORD style);
	// You must use std::array iterators, use -1 to align the column to the right
	void SetParts(int* elements, int parts, bool stretch = true);
	void SetText(const wchar_t* text, int index) 
	{
		if (index > m_nParts || index < NULL) {
			return;
		}
		this->Notify<int, LPCWSTR>(SB_SETTEXT, MAKEWORD(index, SBT_POPOUT), text);
	};
	void SizeChanged() 
	{ 
		this->Notify(WM_SIZE, 0, 0);
	}
	const wchar_t* getClass() override { return STATUSCLASSNAME; }
};

#endif