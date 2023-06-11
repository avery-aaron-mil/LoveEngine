#include <love/common/data/files/logger.hpp>
#include <love/common/system/device_info.hpp>

#include <love/client/client_instance.hpp>

#include "client/client_states/client_state_loading.hpp"

#include <stdlib.h>

using namespace love_engine;
using namespace example_game;

void log_Device_Info(const Logger& logger) {
    Device_Info device_Info;

    char device_Message[512];
    std::snprintf(
        device_Message, sizeof(device_Message),
        "Device Information:\n"
        "\tOS: %s\n"
        "\tCPU: %s\n"
        "\tVideo Card: %s\n"
        "\tPhysical Memory (RAM): %lluB\n",
        device_Info.get_OS().c_str(),
        device_Info.get_CPU().c_str(),
        device_Info.get_Video_Card().c_str(),
        device_Info.get_Physical_Memory()
    );
    logger.log(Log_Status::INFO, device_Message);
}

int main(int argc, char** argv) {
    Logger logger("logs/latest.log");
    log_Device_Info(logger);
    
    Client_State_Loading loading_State;
    Client_Instance client(&loading_State, 50.f);
    client.run();

    exit(EXIT_SUCCESS);
}
