#include "love_engine_instance.hpp"

#include <csignal>
#include <stack>
#include <thread>

#include "error/crash.hpp"
#include "system/system_info.hpp"
#include "system/thread.hpp"

namespace love_engine {
    std::stack<std::function<void()>> _callbacks;

    [[noreturn]] void _signal_Handler(int signum) {
        switch (signum) {
            case SIGINT:
                Crash::crash("(SIGINT) Received interrupt signal.");
            case SIGILL:
                Crash::crash("(SIGILL) Illegal processor instruction.");
            case SIGFPE:
                Crash::crash("(SIGFPE) Floating point exception caused by overflow/underflow or division by zero.");
            case SIGSEGV:
                Crash::crash("(SIGSEGV) Attempted to read/write memory whose address was not allocated or accessible.");
            case SIGABRT:
                Crash::crash("(SIGABRT) Abort signal was raised.");
            case SIGTERM:
                Crash::crash("(SIGTERM) Received termination signal. Closing.");
            default:
                Crash::crash("Unknown signal was raised.");
        }
    }

    void _set_Signal_Handler() {
        std::signal(SIGINT, _signal_Handler);
        std::signal(SIGILL, _signal_Handler);
        std::signal(SIGSEGV, _signal_Handler);
        std::signal(SIGFPE, _signal_Handler);
        std::signal(SIGABRT, _signal_Handler);
        std::signal(SIGTERM, _signal_Handler);
    }

    void LoveEngineInstance::init(const std::string& crashDirectory) {
        _set_Signal_Handler();
        Thread::register_Thread(std::this_thread::get_id(), "Main");
        SystemInfo::get_Consolidated_System_Info();
        FileIO::get_Executable_Directory();
        Crash::set_Crash_Directory(crashDirectory);
    }
    
    void LoveEngineInstance::cleanup() {
        Thread::wait_For_Threads();
        while (!_callbacks.empty()) {
            _callbacks.top()();
            _callbacks.pop();
        }
    }
    
    void LoveEngineInstance::add_Exit_Callback(std::function<void()> callback) {
        _callbacks.push(callback);
    }
}
