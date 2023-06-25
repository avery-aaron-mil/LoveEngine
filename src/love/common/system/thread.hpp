#ifndef LOVE_THREADS_HPP
#define LOVE_THREADS_HPP

#include <functional>
#include <string>
#include <thread>

namespace love_engine {
    class Thread {
        public:
            template<class F, class... Args>
            Thread(const std::string& name, F&& f, Args&&... args) {
                _thread = std::thread(
                    [](auto&&... fwd){
                        _handle_Thread(std::forward<decltype(fwd)>(fwd)...);
                    },
                    name,
                    std::forward<F>(f),
                    std::forward<Args>(args)...
                );
            }

            Thread(Thread const&) = delete;
            void operator=(Thread const&) = delete;
            ~Thread() {
                _thread.detach();
            }

            std::thread::id get_id() const noexcept { return _thread.get_id(); }
            void join() noexcept { if (_thread.joinable()) _thread.join(); }

            std::string get_Thread_Name() const noexcept {
                return get_Thread_Name(get_id());
            }
            static std::string get_Thread_Name(const std::thread::id& id);

            void rename_Thread(const std::string& name) noexcept {
                register_Thread(get_id(), name);
            }
            static void rename_Thread(const std::thread::id& id, const std::string& name) {
                register_Thread(id, name);
            }

            // NOTE: Recommended to not use this functions unless native threads are absolutely necessary.
            static void register_Thread(const std::thread::id& id, const std::string& name);
            // NOTE: Recommended to not use this functions unless native threads are absolutely necessary.
            static void unregister_Thread(const std::thread::id& id);

        private:
            template<class F, class... Args>
            static void _handle_Thread(const std::string name, F&& f, Args&&... args) {
                register_Thread(std::this_thread::get_id(), name);
                f(args...);
                unregister_Thread(std::this_thread::get_id());
            }

            std::thread _thread;
    };
}

#endif // LOVE_THREADS_HPP
