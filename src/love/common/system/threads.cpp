#include "threads.hpp"

#include <unordered_map>

namespace love_engine {
    std::unordered_map<std::thread::id, std::string> Threads::_threads;

    explicit std::thread Threads::create_Thread(const std::string& name, Function&& f, Args&&... args) {
        std::thread thread(f, args);
        register_Thread(thread.get_id(), name);
        std::thread parent(_unregister_On_Finish, thread.get_id());
        return thread;
    }

    std::string Threads::get_Thread_Name(const std::thread::id& id) {
        return _threads.contains(id) ? _threads.find(id)->second : "UNREGISTERED_THREAD";
    }

    void Threads::register_Thread(const std::thread::id& id, const std::string& name) {
        _threads[id] = name;
    }
    void Threads::unregister_Thread(const std::thread::id& id) {
        _threads.erase(id);
    }

    void Threads::_unregister_On_Finish(const std::thread& thread) {
        thread.join();
        unregister_Thread(thread.get_id());
    }
}
