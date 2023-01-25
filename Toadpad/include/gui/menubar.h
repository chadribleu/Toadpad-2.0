#ifndef MENUBAR_H
#define MENUBAR_H

#include "window.h"
#include "menu.h"
#include "common.h"

class Window;
class Menu;

class MenuBar 
{
public:
	MenuBar();
	MenuBar(Window* p_parent);
	~MenuBar();
	
	inline void SetParent(Window* p_parent) { parent = p_parent; }
	inline Window* GetParent() const { return parent; }
	inline HMENU GetHandle() const { return self; }

	void AddMenu(const wchar_t* p_name, Menu* p_menu);

private:
	Window* parent;
	HMENU self;
	std::vector<Menu*> m_children;
};

#endif