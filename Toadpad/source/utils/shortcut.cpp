#include "utils/shortcut.h"

Shortcut::Shortcut(Shortcut::Modifier modifier, wchar_t key) : m_struct{ 0 }
{
	this->SetData(modifier, key);
}

void Shortcut::SetData(Shortcut::Modifier modifier, wchar_t key)
{
	m_struct.fVirt = (static_cast<BYTE>(modifier)) | FVIRTKEY;
	m_struct.key = key;
}

std::wstring Shortcut::ToStr()
{ // fVirt = '\t'
	std::wstring buff;

	if (m_struct.fVirt & Shortcut::Modifier::CTRL)
	{
		buff += L"Ctrl+";
	}
	
	if (m_struct.fVirt & Shortcut::Modifier::ALT)
	{
		buff += L"Alt+";
	}
	
	if (m_struct.fVirt & Shortcut::Modifier::SHIFT)
	{
		buff += L"Shift+";
	}

	return std::wstring(buff + static_cast<wchar_t>(std::towupper(static_cast<wchar_t>(m_struct.key))));
}