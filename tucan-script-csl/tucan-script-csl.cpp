#include <fstream>
#include <iterator>
#include <iostream>
#include <vector>
#include "tucan_module.h"

int main()
{
    std::string filename;
    std::cin >> filename;

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    auto mod = std::make_shared<tucan_script::tucan_module>();

    HMODULE plugin = LoadLibraryA("tucan-script-plugin-sample.dll");

    if (!plugin)
    {
        std::cerr << "Unable to load plugin" << std::endl;
        return 2;
    }

    mod->load_plugin(plugin);
    mod->load_from_source(src);
    mod->execute();

    FreeLibrary(plugin);

    return 0;
}