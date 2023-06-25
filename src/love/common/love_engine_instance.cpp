#include "love_engine_instance.hpp"

#include <thread>

#include "data/files/file_io.hpp"
#include "system/system_info.hpp"
#include "system/thread.hpp"

namespace love_engine {
    void LoveEngineInstance::init() {
        Thread::register_Thread(std::this_thread::get_id(), "Main");
        SystemInfo::get_Consolidated_System_Info();
        FileIO::get_Executable_Directory();
    }
    
    void LoveEngineInstance::cleanup() {
        Thread::wait_For_Threads();
    }
}
