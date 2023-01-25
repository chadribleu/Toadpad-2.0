#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef _UNICODE
#define APPNAME L"Toadpad"
#else
#define APPNAME "Toadpad"
#endif

#define CHOOSEMIN(a, b) ((a) < (b) ? (a) : (b))
#define CHOOSEMAX(a, b) ((a) > (b) ? (a) : (b))
#define INVERTSIGN(a) a * (-1)

// WPARAM - Returns the current line
// LPARAM - Returns the current column
#define CEM_GETLINEINFO			(WM_USER + 0x0001)
// WPARAM - Returns numerator
// LPARAM - Returns denominator
#define CEM_ZOOMCHANGED			(WM_USER + 0x0002)
// WPARAM - The high order byte returns a pointer to the Window which subclassed occurred, and the lo-order byte returns a pointer to the callback function.
// LPARAM - Returns 0
#define CWM_SUBCLASSED			(WM_USER + 0x0003)
#define CTM_ONCOMMAND			(WM_USER + 0x0004)

#endif