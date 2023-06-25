#include "love_engine_instance.hpp"

#include <thread>

#include "data/files/file_io.hpp"
#include "error/crash.hpp"
#include "system/system_info.hpp"
#include "system/thread.hpp"

namespace love_engine {
    void LoveEngineInstance::init(const std::string& crashDirectory) {
        Thread::register_Thread(std::this_thread::get_id(), "Main");
        SystemInfo::get_Consolidated_System_Info();
        FileIO::get_Executable_Directory();
        Crash::set_Crash_Directory(crashDirectory);
    }
    
    void LoveEngineInstance::cleanup() {
        Thread::wait_For_Threads();
    }
}
