#include <love/common/love_engine_instance.hpp>
#include <love/common/data/files/logger.hpp>
#include <love/common/system/system_info.hpp>

#include <love/client/client_instance.hpp>
#include <love/client/graphics/graphics_instance.hpp>

#include "client/client_states/client_state_loading.hpp"

#include <cstdlib>
#include <memory>

using namespace love_engine;
using namespace example_game;

std::shared_ptr<Logger> logger;

void glfwCallback(int e, const char*desc) {
    logger.get()->log("System Info:\n" + SystemInfo::getConsolidatedSystemInfo());
}

int main(int argc, char** argv) {
    LoveEngineInstance loveEngine{FileIO::getExecutableDirectory() + "../crash-reports"};
    logger = std::make_shared<Logger>(Logger(FileIO::getExecutableDirectory() + "../logs/latest.log", true));
    logger.get()->log("System Info:\n" + SystemInfo::getConsolidatedSystemInfo());

    GraphicsInstance::ApplicationInfo applicationInfo{
        .name = "Example Game",
        .versionMajor = 1,
        .versionMinor = 0,
        .versionPatch = 0,
        .logger = logger
    };

    ClientState_Loading loading_State;
    ClientInstance client(&loading_State, ClientInstance::Settings{
        .applicationInfo = applicationInfo,
        .glfwErrorCallback = glfwCallback,
        .msPerTick = 50.f
    });
    client.run();

    exit(EXIT_SUCCESS);
}
