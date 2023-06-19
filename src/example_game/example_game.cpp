#include <love/common/love_engine_instance.hpp>
#include <love/common/data/files/logger.hpp>
#include <love/common/system/system_info.hpp>

#include <love/client/client_instance.hpp>

#include "client/client_states/client_state_loading.hpp"

#include <cstdlib>

#include <thread> // TODO Remove
#include <love/common/system/threads.hpp>

using namespace love_engine;
using namespace example_game;

void test() {
    Logger::log("Hello from test thread!");
}

int main(int argc, char** argv) {
    LoveEngineInstance::init();
    Logger logger(FileIO::get_Executable_Directory() + "../logs/latest.log");
    SystemInfo systemInfo;
    logger.log(systemInfo.get_Consolidated_System_Info());

    Logger::log("Hello from main thread!");
    std::thread testThread = Threads::create_Thread("TestThread", test);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    Logger::log(Threads::get_Thread_Name(testThread.get_id()));

    /*ClientState_Loading loading_State;
    ClientInstance client(&loading_State, 50.f);
    client.run();*/

    exit(EXIT_SUCCESS);
}
