#include "love_engine_instance.hpp"

#include <csignal>
#include <stack>
#include <thread>

#include "error/crash.hpp"
#include "system/system_info.hpp"
#include "system/thread.hpp"

namespace love_engine {
    std::stack<std::function<void()>> _callbacks;

    [[noreturn]] void _signalHandler(int signum) {
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

    void _setSignalHandler() noexcept {
        std::signal(SIGINT, _signalHandler);
        std::signal(SIGILL, _signalHandler);
        std::signal(SIGSEGV, _signalHandler);
        std::signal(SIGFPE, _signalHandler);
        std::signal(SIGABRT, _signalHandler);
        std::signal(SIGTERM, _signalHandler);
    }

    void LoveEngineInstance::_init(const std::string& crashDirectory) noexcept {
        _setSignalHandler();
        Thread::registerThread(std::this_thread::get_id(), "Main");
        SystemInfo::getConsolidatedSystemInfo();
        FileIO::getExecutableDirectory();
        Crash::setCrashDirectory(crashDirectory);
    }

    void LoveEngineInstance::cleanup() noexcept {
        Thread::waitForThreads();
        while (!_callbacks.empty()) {
            _callbacks.top()();
            _callbacks.pop();
        }
    }
    
    void LoveEngineInstance::addExitCallback(const std::function<void()>& callback) noexcept {
        _callbacks.push(callback);
    }

    
    LoveEngineInstance::LoveEngineInstance() { _init(FileIO::getExecutableDirectory() + "crash-reports"); }
    LoveEngineInstance::LoveEngineInstance(const std::string& crashDirectory) { _init(crashDirectory); }
    LoveEngineInstance::~LoveEngineInstance() { cleanup(); }
}
