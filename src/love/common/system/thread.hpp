#ifndef LOVE_THREADS_HPP
#define LOVE_THREADS_HPP
#include <love/common/love_common.hpp>

#include <functional>
#include <string>
#include <thread>

namespace love_engine {
    class LV_ENG_COM Thread {
        public:
            template<class F, class... Args>
            Thread(const std::string& name, F&& f, Args&&... args) {
                _addToThreadCount(); // NOTE: Must be done asynchronously to avoid race conditions.
                _thread = std::thread(
                    [](auto&&... fwd){
                        _handleThread(std::forward<decltype(fwd)>(fwd)...);
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

            std::thread::id getID() const noexcept { return _thread.get_id(); }
            void join() noexcept { if (_thread.joinable()) _thread.join(); }

            std::string getThreadName() const noexcept {
                return getThreadName(getID());
            }
            static std::string getThreadName(const std::thread::id& id) noexcept;

            void renameThread(const std::string& name) noexcept {
                registerThread(getID(), name);
            }
            static void renameThread(const std::thread::id& id, const std::string& name) {
                registerThread(id, name);
            }

            // NOTE: Recommended to not use this functions unless native threads are absolutely necessary.
            static void registerThread(const std::thread::id& id, const std::string& name) noexcept;
            // NOTE: Recommended to not use this functions unless native threads are absolutely necessary.
            static void unregisterThread(const std::thread::id& id) noexcept;

            // NOTE: Should only be called by main thread.
            static void waitForThreads() noexcept;

        private:
            template<class F, class... Args>
            static void _handleThread(const std::string name, F&& f, Args&&... args) {
                registerThread(std::this_thread::get_id(), name);
                f(args...);
                unregisterThread(std::this_thread::get_id());
            }

            static void _addToThreadCount() noexcept;

            std::thread _thread;
    };
}

#endif // LOVE_THREADS_HPP
