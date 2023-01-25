#include "utils.h"

// @brief Copy a block of memory to another location and add a NULL character at the end of the string
wchar_t* Utils::StringCopy(wchar_t* destination, const wchar_t* source, unsigned count) {
	unsigned int counter = 0;

	wchar_t character = NULL;
	while (counter != (count - 1)) {
		destination[counter] = source[counter];
		character = source[counter];
		++counter;
	}
	destination[counter] = L'\0';
	return destination;
}

// Loop over the given string until a '\0' character is hitted.
unsigned int Utils::StringSize(const wchar_t* source) {
	unsigned int size = 0;

	wchar_t character = L' ';
	while (character != L'\0') {
		character = source[size];
		++size;
	}
	return size;
}