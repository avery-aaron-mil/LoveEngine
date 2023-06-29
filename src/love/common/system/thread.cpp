#include "thread.hpp"

#include <chrono>
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace love_engine {
    std::mutex _threadsMutex;
    volatile size_t _openThreads = 1; // NOTE: Setting to "1" accounts for main thread.
    std::unordered_map<std::thread::id, std::string> _threadNames;

    std::string Thread::get_Thread_Name(const std::thread::id& id) noexcept {
        _threadsMutex.lock();
        std::string name = _threadNames.contains(id) ? _threadNames[id] : "UNREGISTERED_THREAD";
        _threadsMutex.unlock();
        return name;
    }

    void Thread::register_Thread(const std::thread::id& id, const std::string& name) noexcept {
        _threadsMutex.lock();
        _threadNames[id] = name;
        _threadsMutex.unlock();
    }

    void Thread::unregister_Thread(const std::thread::id& id) noexcept {
        _threadsMutex.lock();
        if (_threadNames.contains(id)) _threadNames.erase(id);
        _threadsMutex.unlock();
        --_openThreads;
    }

    void Thread::wait_For_Threads() noexcept {
        unregister_Thread(std::this_thread::get_id());

        constexpr int MS_PER_TRY = 100;
        constexpr int MAX_TRIES = 5000 / MS_PER_TRY; // 5 seconds, 50 tries
        int tries = 0;
        while ((_openThreads > 0) && (++tries <= MAX_TRIES)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_TRY));
        }
    }

    void Thread::_add_To_Thread_Count() noexcept {
        ++_openThreads;
    }
}
