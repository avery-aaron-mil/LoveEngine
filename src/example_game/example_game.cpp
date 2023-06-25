#include <love/common/love_engine_instance.hpp>
#include <love/common/data/files/logger.hpp>
#include <love/common/system/system_info.hpp>

#include <love/client/client_instance.hpp>

#include "client/client_states/client_state_loading.hpp"

#include <cstdlib>

#include <thread> // TODO Remove
#include <love/common/system/thread.hpp>

using namespace love_engine;
using namespace example_game;

void test() {
    Logger logger(FileIO::get_Executable_Directory() + "../logs/latest.log");
    logger.log("Hello from test thread!");
}

int main(int argc, char** argv) {
    LoveEngineInstance::init();
    Logger logger(FileIO::get_Executable_Directory() + "../logs/latest.log", true);
    SystemInfo systemInfo;
    logger.log("System Info:\n" + systemInfo.get_Consolidated_System_Info());

    logger.log("Hello from main thread!");
    Thread testThread("Test", test);

    /*ClientState_Loading loading_State;
    ClientInstance client(&loading_State, 50.f);
    client.run();*/

    LoveEngineInstance::cleanup();
    exit(EXIT_SUCCESS);
}
