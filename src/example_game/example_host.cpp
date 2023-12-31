#include <love/common/love_engine_instance.hpp>
#include <love/common/data/files/logger.hpp>
#include <love/common/system/system_info.hpp>

#include <cstdlib>

using namespace love_engine;

int main(int argc, char** argv) {
    LoveEngineInstance::init();
    Logger logger(FileIO::get_Executable_Directory() + "../logs/latest.log", true);
    logger.log("System Info:\n" + SystemInfo::get_Consolidated_System_Info());

    LoveEngineInstance::cleanup();
    exit(EXIT_SUCCESS);
}
