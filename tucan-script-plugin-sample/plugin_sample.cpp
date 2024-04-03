#include "plugin_sample.h"

__declspec(dllexport) void tucan_load_plugin(tucan_script::tucan_module* modulePtr)
{
	modulePtr->load_external_executable("hello", [](tucan_script::tucan_function& fun) {
		std::cout << "Hello from plugin!" << std::endl;
		});
}
