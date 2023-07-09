#include <love/common/love_engine_instance.hpp>
#include <love/common/data/files/logger.hpp>
#include <love/common/system/system_info.hpp>

#include <cstdlib>

using namespace love_engine;

int main(int argc, char** argv) {
    LoveEngineInstance loveEngine{};
    Logger logger(FileIO::getExecutableDirectory() + "../logs/latest.log", true);
    logger.log("System Info:\n" + SystemInfo::getConsolidatedSystemInfo());

    exit(EXIT_SUCCESS);
}
