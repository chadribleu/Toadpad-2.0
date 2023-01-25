#ifndef LISTBOX_H
#define LISTBOX_H

#include "gui/window.h"

class ListBox : public Window
{
public:
	ListBox(Window* parent = nullptr, int identifier = 0);
	virtual ~ListBox();
	const wchar_t* getClass() override { return L"LISTBOX"; }
	void FillItems(const std::vector<const wchar_t*>& strings);

private:

};

#endif
