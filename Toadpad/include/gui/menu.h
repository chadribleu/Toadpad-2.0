#ifndef MENU_H
#define MENU_H

#include "common.h"
#include "menubar.h"
#include "menuitem.h"
#include "../utils/shortcut.h"

class MenuBar;
class MenuItem;

class Menu
{
public:
	Menu();
	~Menu();

	MenuItem* AddItem(const std::wstring name, UINT_PTR item_identifier, Shortcut& shortcut);
	MenuItem* AddItem(const std::wstring name, UINT_PTR item_identifier);

	// Getter
	inline HMENU GetParent() const { return parentBar; }
	inline HMENU GetHandle() const { return self; }

	void AddMenu(const wchar_t* p_name, const Menu& p_menu);
	void AddSeparator();
	void SetParent(const MenuBar* p_parent);

private:
	HMENU parentBar;
	HMENU self;
	std::vector<MenuItem*> mi;
};

#endif