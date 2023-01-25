#include "pch.h"
#include "FontPicker.h"

FontPicker::FontPicker(HWND owner, DWORD flags) : m_internalErr(0)
{
	memset(&m_cf, 0, sizeof(CHOOSEFONT));
	m_cf.lStructSize = sizeof(CHOOSEFONT);
	m_cf.hwndOwner = m_owner;
	m_cf.Flags = flags;
}

bool FontPicker::ShowDialog(LOGFONT* default_font, COLORREF def_rgb_data, LimitSize min_max_size)
{
	if (default_font != nullptr) {
		m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
		m_lf = *default_font;
	}
	if (min_max_size) {
		m_cf.Flags |= CF_LIMITSIZE;
		m_cf.nSizeMin = min_max_size.min;
		m_cf.nSizeMax = min_max_size.max;
	}
	m_cf.lpLogFont = &m_lf;
	m_cf.rgbColors = def_rgb_data;
	m_internalErr = ChooseFont(&m_cf);
	return m_internalErr;
}

HFONT FontPicker::GetFont() const
{
	return CreateFontIndirect(m_cf.lpLogFont);
}