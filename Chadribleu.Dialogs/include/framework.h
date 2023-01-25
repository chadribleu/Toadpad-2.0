#pragma once

#ifdef CHADRIBLEUDIALOGS_EXPORTS
#define CHADRIBLEUDIALOGS_API __declspec(dllexport)
#else
#define CHADRIBLEUDIALOGS_API __declspec(dllimport)
#endif

#include <Windows.h>
#include <ShObjIdl_core.h>
#include <ShlObj_core.h>
#include <stdexcept>