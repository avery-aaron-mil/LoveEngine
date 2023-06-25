#include "thread.hpp"

#include <mutex>
#include <unordered_map>

namespace love_engine {
    std::mutex _threadsMutex;
    std::unordered_map<std::thread::id, std::string> _threads;

    std::string Thread::get_Thread_Name(const std::thread::id& id) {
        _threadsMutex.lock();
        std::string name = _threads.contains(id) ? _threads[id] : "UNREGISTERED_THREAD";
        _threadsMutex.unlock();
        return name;
    }

    void Thread::register_Thread(const std::thread::id& id, const std::string& name) {
        _threadsMutex.lock();
        _threads[id] = name;
        _threadsMutex.unlock();
    }
    void Thread::unregister_Thread(const std::thread::id& id) {
        _threadsMutex.lock();
        if (_threads.contains(id)) _threads.erase(id);
        _threadsMutex.unlock();
    }
}
