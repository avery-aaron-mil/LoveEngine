#include <love/common/love_engine_instance.hpp>
#include <love/common/data/files/logger.hpp>
#include <love/common/system/system_info.hpp>

#include <love/client/client_instance.hpp>

#include "client/client_states/client_state_loading.hpp"

#include <cstdlib>
#include <memory>

using namespace love_engine;
using namespace example_game;

std::shared_ptr<Logger> logger;

void glfwCallback(int e, const char*desc) {
    logger.get()->log("System Info:\n" + SystemInfo::get_Consolidated_System_Info());
}

int main(int argc, char** argv) {
    LoveEngineInstance::init(FileIO::get_Executable_Directory() + "../crash-reports");
    logger = std::make_shared<Logger>(Logger(FileIO::get_Executable_Directory() + "../logs/latest.log", true));
    logger.get()->log("System Info:\n" + SystemInfo::get_Consolidated_System_Info());

    ClientState_Loading loading_State;
    ClientInstance client(&loading_State, ClientInstance::Settings{.glfwErrorCallback = glfwCallback, .msPerTick = 50.f});
    client.run();

    LoveEngineInstance::cleanup();
    exit(EXIT_SUCCESS);
}
