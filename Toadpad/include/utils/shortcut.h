#ifndef SHORTCUT_H
#define SHORTCUT_H

#include "common.h"

class Shortcut
{
public:
	enum Modifier : BYTE
	{
		CTRL = FCONTROL,
		ALT = FALT,
		SHIFT = FSHIFT
	};

	Shortcut() = default;
	Shortcut(Shortcut::Modifier modifier, wchar_t key);
	void SetData(Shortcut::Modifier modifier, wchar_t key);
	ACCEL* GetStruct() { return &m_struct; };
	std::wstring ToStr();

private:
	ACCEL m_struct;
};

inline Shortcut::Modifier operator|(Shortcut::Modifier a, Shortcut::Modifier b)
{
	return static_cast<Shortcut::Modifier>(static_cast<int>(a) | static_cast<int>(b));
}

#endif