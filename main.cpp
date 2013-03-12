#include <iostream>
#include "scene_lua.hpp"
#include "worker.hpp"

extern Worker worker;

int main(int argc, char** argv)
{
    char * run_type = getenv("RUN_TYPE");
    string runType(run_type);

    while (true) {
        if (runType == "WORKER") {
            worker.accept();
        }

        std::string filename = "scene.lua";
        if (argc >= 2) {
            filename = argv[1];
        }

        if (!run_lua(filename)) {
            std::cerr << "Could not open " << filename << std::endl;
            return 1;
        }

        if (runType == "COORDINATOR") {
            break;
        }
    }

    return 0;
}

