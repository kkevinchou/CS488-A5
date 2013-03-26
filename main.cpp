#include <iostream>
#include "scene_lua.hpp"
#include "worker.hpp"

extern Worker worker;

int main(int argc, char** argv)
{
    char * run_type = getenv("RUN_TYPE");
    string runType;


    if (run_type == NULL) {
        runType = "NORMAL";
    } else {
        runType = string(run_type);
    }

    bool firstRun = false;
    while (true) {
        if (firstRun) {
            cerr << "RUN TYPE: " << runType << endl;
        }

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

        if (runType != "WORKER") {
            break;
        }

        firstRun = false;
    }

    return 0;
}

