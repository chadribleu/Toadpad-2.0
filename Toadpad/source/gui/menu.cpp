#include "gui/menu.h"

Menu::Menu() : self(CreatePopupMenu()) {}

Menu::~Menu() 
{
	size_t arrSize = mi.size();
	for (int i = 0; i < arrSize; ++i) {
		delete mi[i];
	}
}

MenuItem* Menu::AddItem(const std::wstring name, UINT_PTR p_identifier, Shortcut& shortcut)
{
	AppendMenuW(self, MF_STRING, p_identifier, name.c_str());
	MenuItem* item = new MenuItem(name, p_identifier, shortcut);
	int index = (int)mi.size();
	item->index = index;
	item->parent = self;
	item->AssignShortcut(shortcut);
	mi.push_back(item);
	return item;
}

MenuItem* Menu::AddItem(const std::wstring name, UINT_PTR p_identifier) 
{
	AppendMenuW(self, MF_STRING, p_identifier, name.c_str());
	MenuItem* item = new MenuItem(name, p_identifier);
	int index = static_cast<int>(mi.size());
	item->index = index;
	item->parent = self;
	mi.push_back(item);
	return item;
}

void Menu::AddSeparator() 
{
	AppendMenuW(self, MF_SEPARATOR, 0, NULL);
	MenuItem* itmEmpty = new MenuItem(L"MENU_SEPARATOR", 0);
	itmEmpty->index = static_cast<int>(mi.size());
	itmEmpty->parent = self;
	mi.push_back(itmEmpty);
}

void Menu::SetParent(const MenuBar* p_parent)
{
	parentBar = p_parent->GetHandle();
}

void Menu::AddMenu(const wchar_t* p_name, const Menu& p_menu) 
{
	AppendMenuW(self, MF_POPUP, (UINT_PTR)p_menu.self, p_name);
	MenuItem* itmMenu = new MenuItem(L"MENU_MENU", 0);
	itmMenu->index = static_cast<int>(mi.size());
	itmMenu->parent = self;
	mi.push_back(itmMenu);
}