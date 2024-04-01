#include "tucan_domain.h"

TUCAN_EXPORT tucan_script::tucan_module* tucan_init_domain()
{
    return new tucan_script::tucan_module();
}

TUCAN_EXPORT void tucan_load_from_source(tucan_script::tucan_module* module_ptr, LPCSTR src)
{
    module_ptr->load_from_source(std::string(src));
}

TUCAN_EXPORT void tucan_load_plugin(tucan_script::tucan_module* module_ptr, HINSTANCE hDll)
{
    tucan_load_plugin_fun plugin_function_ptr = (tucan_load_plugin_fun) GetProcAddress(hDll, "tucan_load_plugin");
    plugin_function_ptr(module_ptr);
}



