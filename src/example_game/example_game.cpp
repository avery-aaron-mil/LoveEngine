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

void test(const Logger logger) {
    logger.log("Hello from test thread!");
}

int main(int argc, char** argv) {
    LoveEngineInstance::init();
    Logger logger(FileIO::get_Executable_Directory() + "../logs/latest.log");
    SystemInfo systemInfo;
    logger.log("System Info:\n" + systemInfo.get_Consolidated_System_Info());

    logger.log("Hello from main thread!");
    Thread testThread("Test", test, logger);
    testThread.join();
    logger.log(Thread::get_Thread_Name(testThread.get_id()));

    /*ClientState_Loading loading_State;
    ClientInstance client(&loading_State, 50.f);
    client.run();*/

    exit(EXIT_SUCCESS);
}
