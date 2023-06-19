#ifndef LOVE_THREADS_HPP
#define LOVE_THREADS_HPP

#include <string>
#include <thread>

namespace love_engine {
    class Threads {
        public:
            template<class Function, class... Args>
            static std::thread create_Thread(const std::string& name, Function&& f, Args&&... args);
            static std::string get_Thread_Name(const std::thread::id& id);
            static void rename_Thread(const std::thread::id& id, const std::string& name) {
                register_Thread(id, name);
            }

            // NOTE: Recommended to not use this functions unless native threads are absolutely necessary
            static void register_Thread(const std::thread::id& id, const std::string& name);
            // NOTE: Recommended to not use this functions unless native threads are absolutely necessary
            static void unregister_Thread(const std::thread::id& id);

        private:
            static void _unregister_On_Finish(std::thread& thread);
    };
}

#endif // LOVE_THREADS_HPP
