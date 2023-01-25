#include "gui/ListBox.h"

ListBox::ListBox(Window* parent, int identifier)
{
	this->create(L"list_box", 0, 0, 250, 350, LBS_HASSTRINGS | LBS_STANDARD, 0, parent, reinterpret_cast<HMENU>(identifier));
}

ListBox::~ListBox()
{
}

void ListBox::FillItems(const std::vector<const wchar_t*>& strings)
{
	const size_t size = strings.size();
	for (int i = 0; i < size; ++i) 
	{
		auto pos = static_cast<int>(this->Notify<WPARAM, const wchar_t*>(LB_ADDSTRING, 0, strings[i]));
		this->Notify<int, int>(LB_SETITEMDATA, pos, i);
	}
}