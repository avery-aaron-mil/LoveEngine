#include <love/common/data/files/loggers/standard_logger.hpp>

#include <love/client/client_instance.hpp>

int main(int argc, char** argv) {
    Logger logger = Standard_Logger();
    System_Info system_Info;
    logger.log(Format_String::format_Str("OS: %s", system_Info.OS_Name()));
    logger.log(Format_String::format_Str("CPU: %s", system_Info.CPU_Name()));
    logger.log(Format_String::format_Str("Video Card: %s", system_Info.video_Card_Name()));
    logger.log(Format_String::format_Str("Physical Memory: %ldB", system_Info.physical_Memory()));
    
    Client_Instnace client{loading_State, 50.f};
    client.run();
}
