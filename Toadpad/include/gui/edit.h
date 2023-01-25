#pragma once

#include "window.h"

constexpr int def_width			= 200;
constexpr int def_heigth		= 20;
constexpr int zoom_default_val	= 100;
constexpr int zoom_range		= 10;
constexpr int max_zoom			= 500;
constexpr int min_zoom			= 10;
constexpr int wheel_delta		= 120;

class Edit : public Window 
{
public:
	struct SelectionRange {
		unsigned int begin;
		int end;
	};

	struct CaretPosition {
		unsigned int line;
		unsigned int column;
	};

	struct ZoomInfos {
		int numerator;
		int denominator;
	};

	Edit(Window* parent = nullptr, int width = def_width, int height = def_heigth, const wchar_t* text = L"",
		DWORD style = ES_MULTILINE, DWORD exStyle = 0UL, const int identifier = 0);
	~Edit() final;

	bool enableWordWrap(bool wordwrap);
	void swapMem(HLOCAL buff);

	// Selection
	SelectionRange getSelection();
	CaretPosition GetLineInfo() const;

	void SetSelection(SelectionRange const& range);
	void ReplaceSel(const std::wstring& string, bool canUndo = true);
	void undoText();
	void SetZoom(int percentage = 10);
	ZoomInfos GetZoom() const;

	bool WordWrap() const { return m_wordwrap; }

	const wchar_t* getClass() override { return L"EDIT"; }
	std::wstring GetSelText();

private:
	static LRESULT CALLBACK s_EditSubclassProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR identifier, DWORD_PTR refData);
	LRESULT HandleMessage(UINT, WPARAM, LPARAM);
	SelectionRange m_currSel;
	CaretPosition m_caretPos = { 0 }; // Update this variable in the subclass method
	bool m_wordwrap = false;
	HFONT m_hSubstituteFnt;
	ZoomInfos m_zInfos = { zoom_default_val, zoom_default_val };
};