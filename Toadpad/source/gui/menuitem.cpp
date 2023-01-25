#include "gui/menuitem.h"

MenuItem::MenuItem(std::wstring item_name, UINT_PTR assign_id, Shortcut& shortcut)
	: identifier(assign_id), hUncheckedBmp(NULL), hCheckedBmp(NULL), enabled(true), checked(false), _break(false)
{
	wcscpy_s(name, item_name.c_str());
	this->AssignShortcut(shortcut);
}

MenuItem::MenuItem(std::wstring item_name, UINT_PTR assign_id)
	: identifier(assign_id), hUncheckedBmp(NULL), hCheckedBmp(NULL), enabled(true), checked(false), _break(false)
{
	wcscpy_s(name, item_name.c_str());
}

MenuItem::~MenuItem()
{
}

void MenuItem::SetEnabled(bool p_enabled) 
{
	UINT db = p_enabled ? MF_ENABLED : MF_GRAYED;
	EnableMenuItem(parent, index, db | MF_BYPOSITION);
	this->enabled = p_enabled;
}

void MenuItem::SetChecked(bool p_checked) 
{
	UINT ck = p_checked ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem(parent, index, ck | MF_BYPOSITION);
	this->checked = p_checked;
}

void MenuItem::Break(bool verticalLine) 
{
	UINT style = verticalLine ? MF_MENUBARBREAK : MF_MENUBREAK;
	ModifyMenuW(parent, index, MF_STRING | style | MF_BYPOSITION, identifier, name);
}

void MenuItem::UnBreak()
{
	ModifyMenuW(parent, index, MF_STRING | MF_BYPOSITION, identifier, name);
}

bool MenuItem::SetBmps(HBITMAP p_checked, HBITMAP p_unchecked) 
{
	if (SetMenuItemBitmaps(parent, index, MF_BYPOSITION, p_unchecked, p_checked) != NULL) {
		hCheckedBmp = p_checked;
		hUncheckedBmp = p_unchecked;
		return true;
	}
	return false;
}

void MenuItem::AssignShortcut(Shortcut& shortcut)
{
	hasShortcut = true;
	m_shortcut = shortcut;
	m_shortcut.GetStruct()->cmd = static_cast<WORD>(this->identifier);
	AcceleratorTable* t = Application::shortcutTable();
	t->AddEntry(m_shortcut); // No command!!!
	this->SetName(name);
}

void MenuItem::SetName(std::wstring name)
{
	std::wstring menu_name(name);
	if (hasShortcut) {
		menu_name += L"\t";
		menu_name += m_shortcut.ToStr();
	}
	ModifyMenuW(parent, index, MF_STRING | MF_BYPOSITION, identifier, menu_name.data());
}