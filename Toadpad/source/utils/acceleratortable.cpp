#include "utils/acceleratortable.h"

AcceleratorTable::AcceleratorTable() : m_nEntries(0), m_handle(NULL) 
{
	m_accelList = new ACCEL[max_entries];
}

// why
AcceleratorTable::AcceleratorTable(AcceleratorTable const& copy)
{
	HACCEL handle = copy.m_handle;
	if (handle)
	{
		this->m_handle = copy.m_handle;
	}

	ACCEL* tmpPtr = new ACCEL[max_entries];

	if (copy.m_nEntries > 0)
	{
		ACCEL* tempPtr = new ACCEL[max_entries];
		memcpy(tempPtr, copy.m_accelList, sizeof(m_accelList) * (copy.m_nEntries));

		if (tempPtr)
		{
			this->m_nEntries = copy.m_nEntries;
			this->m_accelList = tempPtr;
		}
		else
		{
			this->m_nEntries = 0;
		}
	}
}

AcceleratorTable::~AcceleratorTable()
{
	delete[] m_accelList;
}

AcceleratorTable& AcceleratorTable::operator=(AcceleratorTable const& other)
{
	AcceleratorTable table(other);

	this->m_handle = table.m_handle;
	this->m_accelList = table.m_accelList;
	this->m_nEntries = table.m_nEntries;

	return *this;
}

// Get the structure of the specified Shortcut
void AcceleratorTable::AddEntry(Shortcut& sh)
{
	m_accelList[m_nEntries] = *sh.GetStruct();
	++m_nEntries;
}

ACCEL AcceleratorTable::GetEntryAt(int index)
{
	if (index > m_nEntries || index < 0) {
		return {0, 0 ,0 };
	}
	return m_accelList[index];
}

// Call this function after all entries has been added to the application
void AcceleratorTable::Create()
{
	if (!m_handle)
	{
		int size = m_nEntries <= 0 ? 0 : m_nEntries++;
		if (size > 0)
		{
			m_handle = CreateAcceleratorTableW(m_accelList, size);
		}
	}
}