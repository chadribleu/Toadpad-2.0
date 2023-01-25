#ifndef ACCELERATOR_TABLE_H
#define ACCELERATOR_TABLE_H

constexpr int max_entries = 1024;

#include "shortcut.h"

class AcceleratorTable
{
public:
	AcceleratorTable();
	AcceleratorTable(AcceleratorTable const& copy);
	~AcceleratorTable();

	AcceleratorTable& operator=(AcceleratorTable const& other);

	void AddEntry(Shortcut& sh);
	void Create();
	ACCEL GetEntryAt(int index);
	HACCEL GetHandle() const { return m_handle; }

private:
	int m_nEntries;
	ACCEL* m_accelList;
	HACCEL m_handle;
};

#endif