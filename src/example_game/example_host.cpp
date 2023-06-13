#include <love/common/data/files/logger_status.hpp>

#include <love/common/system/system_info.hpp>

#include <stdlib.h>

using namespace love_engine;

int main(int argc, char** argv) {
    Logger logger = Standard_Logger();
    SystemInfo system_Info;
    logger.log(Format_String::format_Str("OS: %s", system_Info.OS_Name()));
    logger.log(Format_String::format_Str("CPU: %s", system_Info.CPU_Name()));
    logger.log(Format_String::format_Str("Video Card: %s", system_Info.video_Card_Name()));
    logger.log(Format_String::format_Str("Physical Memory: %ldB", system_Info.physical_Memory()));
    
    return EXIT_SUCCESS;
}
