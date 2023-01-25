#ifndef FONTPICKER_H
#define FONTPICKER_H

#include "pch.h"

class CHADRIBLEUDIALOGS_API FontPicker
{
private:
	BOOL m_internalErr;
	HWND m_owner;
	LOGFONT m_lf;
	CHOOSEFONT m_cf;
	HFONT m_hFont, m_hPrevFont;

public:
	struct LimitSize {
		int min;
		int max;
		operator bool() const { return (min > 0) && (max > 0); }
	};

	FontPicker(HWND owner = NULL, DWORD flags = CF_EFFECTS | CF_SCREENFONTS);
	bool ShowDialog(LOGFONT* default_font = nullptr, COLORREF def_rgb_data = RGB(0, 0, 0), LimitSize min_max_size = LimitSize{0, 0});
	int GetPointSize() const { return m_cf.iPointSize; }
	WORD GetFontType() const { return m_cf.nFontType; }

	LOGFONT* GetLogicalFontStruct() { return m_cf.lpLogFont; }
	HFONT GetFont() const;
	COLORREF GetSelColor() const { return m_cf.rgbColors; }
};

#endif