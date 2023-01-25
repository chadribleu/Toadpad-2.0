#include <cstdlib>

// TODO: delete this useless file

#define CHOOSEMIN(a, b) ((a) < (b) ? (a) : (b))
#define CHOOSEMAX(a, b) ((a) > (b) ? (a) : (b))
#define INVERTSIGN(a) a * (-1)

namespace Utils 
{
	wchar_t* StringCopy(wchar_t* destination, const wchar_t* source, unsigned count);
	unsigned int StringSize(const wchar_t* source);
}