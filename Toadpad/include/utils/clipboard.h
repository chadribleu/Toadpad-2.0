#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "common.h"
#include "utils.h"
#include "gui/window.h"

class Window;

class Clipboard
{
public:
	Clipboard();
	Clipboard(const Window* parent);
	Clipboard(Clipboard const&) = delete;
	~Clipboard();

	bool Open(bool delayedRendering = false);
	bool Close();
	void SetText(const wchar_t* text);
	std::wstring GetText();
	bool Clear();
	std::vector<UINT> EnumAvailableFormats();

private:
	HWND m_parent;
	bool m_isOpen;
};

#endif