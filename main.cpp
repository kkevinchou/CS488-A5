#include <iostream>
#include "scene_lua.hpp"

extern bool main_first_run;


int main(int argc, char** argv)
{
    main_first_run = true;
    char * run_type = getenv("RUN_TYPE");
    string runType(run_type);

    while (main_first_run || runType == "WORKER") {
        std::string filename = "scene.lua";
        if (argc >= 2) {
            filename = argv[1];
        }

        if (!run_lua(filename)) {
            std::cerr << "Could not open " << filename << std::endl;
            return 1;
        }

        main_first_run = false;
    }
    return 0;
}

