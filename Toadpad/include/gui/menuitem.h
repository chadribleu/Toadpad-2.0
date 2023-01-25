#ifndef MENUITEM_H
#define MENUITEM_H

#include "common.h"
#include "../utils/application.h"
#include "../utils/shortcut.h"

constexpr int MAX_MEMALLOC = 64;

class AcceleratorTable;

class MenuItem 
{
public:
	friend class Menu;

	MenuItem(std::wstring item_name, UINT_PTR assign_id, Shortcut& shortcut);
	MenuItem(std::wstring item_name, UINT_PTR assign_id);
	virtual ~MenuItem();

	// Getter
	inline HMENU GetParent() const { return parent; }
	inline const wchar_t* GetName() const { return name; }
	inline UINT_PTR GetIdentifier() const { return identifier; }
	inline int GetIndex() const { return index; }
	inline bool IsChecked() const { return checked; }
	inline bool IsEnabled() const { return enabled; }

	void SetName(std::wstring name);
	void SetEnabled(bool enable);
	void SetChecked(bool check);
	void Break(bool boolean);
	void UnBreak();
	bool SetBmps(HBITMAP checked_bmp, HBITMAP unchecked_bmp);

	void AssignShortcut(Shortcut& shortcut);

private:
	HMENU parent;
	int index;

	UINT_PTR identifier;
	wchar_t name[MAX_MEMALLOC];

	Shortcut m_shortcut;

	// Bitmaps
	HBITMAP hCheckedBmp;
	HBITMAP hUncheckedBmp;

	// Booleans
	bool enabled;
	bool checked;
	bool _break;
	bool hasShortcut;
};

#endif