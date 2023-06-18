#include <love/common/data/files/logger.hpp>
#include <love/common/system/system_info.hpp>

#include <love/client/client_instance.hpp>

#include "client/client_states/client_state_loading.hpp"

#include <cstdlib>

using namespace love_engine;
using namespace example_game;

int main(int argc, char** argv) {
    Logger logger("latest.log");
    SystemInfo systemInfo;
    logger.log(systemInfo.get_Consolidated_System_Info());

    /*ClientState_Loading loading_State;
    ClientInstance client(&loading_State, 50.f);
    client.run();*/

    exit(EXIT_SUCCESS);
}
