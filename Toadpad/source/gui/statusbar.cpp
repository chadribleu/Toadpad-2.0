#include "gui/statusbar.h"

StatusBar::StatusBar(Window* parent, DWORD style) : m_nParts(0)
{
	this->create(L"", 
		0, 0, 0, 0, 
		style, NULL, parent, NULL);
}

void StatusBar::SetParts(int* elements, int parts, bool stretch)
{
	if (parts > MAXPARTS) {
		return;
	}
	int* partsMem = nullptr;
	m_nParts = parts;
	HLOCAL hMem = LocalAlloc(LHND, m_nParts * sizeof(int));
	if (hMem) {
		partsMem = static_cast<int*>(LocalLock(hMem));
		if (stretch && m_parent != NULL) { // Stretch all parts relative to the size of the parent
			RECT rc{ 0 };
			GetWindowRect(m_parent, &rc);
			int width = rc.right / m_nParts;
			int rEdge = width;
			for (int i = 0; i < m_nParts; ++i) {
				partsMem[i] = rEdge;
				rEdge += width;
			}
		}
		else {
			for (int i = 0; i < m_nParts; ++i) {
				partsMem[i] = elements[i];
			}
		}
		this->Notify<int, int*>(SB_SETPARTS, m_nParts, partsMem);
		LocalUnlock(hMem);
		LocalFree(hMem);
	}
}