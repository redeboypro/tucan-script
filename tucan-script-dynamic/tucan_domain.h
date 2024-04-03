#pragma once

#include <Windows.h>
#include <functional>
#include <memory>
#include "tucan_module.h"

#define TUCAN_EXPORT __declspec(dllexport)

extern "C++"
{
	TUCAN_EXPORT tucan_script::tucan_module* tucan_init_domain();
	TUCAN_EXPORT void tucan_load_from_source(tucan_script::tucan_module* module_ptr, LPCSTR src);
	TUCAN_EXPORT void tucan_load_plugin(tucan_script::tucan_module* module_ptr, HINSTANCE hDll);
}
