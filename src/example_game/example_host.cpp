#include <love/common/data/files/logger.hpp>

#include <love/common/system/system_info.hpp>

#include <stdlib.h>

using namespace love_engine;

int main(int argc, char** argv) {
    Logger logger("logs/host_latest.txt");
    SystemInfo systemInfo;
    logger.log(Log_Status::LOG_INFO, systemInfo.get_Consolidated_System_Info());
    
    exit(EXIT_SUCCESS);
}
