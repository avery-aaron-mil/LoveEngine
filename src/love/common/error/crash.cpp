#include "crash.hpp"

#include "../love_engine_instance.hpp"
#include "../data/files/file_io.hpp"
#include "../system/system_info.hpp"
#include "../system/thread.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <sstream>
#include <stacktrace>
#include <sys/time.h>

namespace love_engine {
    volatile bool _crashed = false;
    std::string _crashDir(".");
    std::string _crashPath;
    void _default_Crash_Function(const std::string& message);
    std::function<void(const std::string&)> _crashFunction = _default_Crash_Function;
    std::vector<std::string> _flavorTexts = {
        "Everything's going to plan. No, really, that was supposed to happen.",
        "Sorry :(",
        "On the bright side, I bought you a teddy bear!",
        "What did you do??",
        "You're mean.",
        "Whoopsie daisy!",
        "Oops.",
        "Who set off the TNT?",
        "I bet Riley wouldn't have this problem.",
        "I'm sorry, Steve.",
        "Would you like a cupcake?",
        "This is why we have a warranty...",
        "I just don't know what went wrong :(",
        "I feel sad now :(",
        "Surprise! Haha. Well, this is awkward.",
        "Wanna talk about it?",
        "Hey, that tickles! Hehehe!",
        "Ooh. Shiny.",
        "Uh... Did I do that?",
        "My bad.",
#ifdef _WIN32
        "I blame Microsoft.",
#elif defined(__APPLE__)
        "I blame Apple.",
#else
        "I blame Linux masterrace.",
#endif
        "Don't be sad. I'll do better next time, I promise!",
        "You've got this!",
        "Hmm...",
        "Maybe try something else next time?",
        "Why did you do that?",
        "Shucks.",
        "No game here! :')",
        "It'll be okay.",
        "Time to surf the web!",
        "Work smarter, not harder.",
        "It's okay to ask for help, you know.",
        "Better luck next time!",
        "I hope your day improves."
    };

    void Crash::set_Crash_Function(std::function<void(const std::string&)> crashFunction) {
        _crashFunction = crashFunction;
    }

    void Crash::set_Crash_Directory(const std::string& directory) {
        _crashDir.assign(directory);
    }
    void Crash::set_Crash_Path(const std::string& filePath) {
        _crashPath.assign(filePath);
    }

    void Crash::set_Flavor_Texts(const std::vector<std::string>& flavorTexts) {
        _flavorTexts = flavorTexts;
    }

    std::string _generate_Crash_Report(const std::string& message, std::tm*& now) {
        // Get time
        char timeBuffer[] = "MM/DD/YYYY HH:mm XM (+SS.UUUUUUs)"; // 2/17/2008 7:35 AM (+20.276508s)

        struct timeval tv;
        if (!gettimeofday(&tv, nullptr)) {
            time_t time = static_cast<time_t>(tv.tv_sec);
            std::srand(tv.tv_sec);
            now = std::localtime(&time);
            if (now != nullptr) {
                std::snprintf(timeBuffer, sizeof(timeBuffer), "%d/%d/%d %d:%02d %s (+%2d.%6lds)",
                    (now->tm_mon + 1), now->tm_mday, (now->tm_year - 100), // Date
                    (now->tm_hour % 12), now->tm_min, ((now->tm_hour < 12) ? "AM" : "PM"), now->tm_sec, tv.tv_usec // Time
                );
            }
        }

        // Set crash message
        // Who/what/when/where/why/how
        std::stringstream outputMessageBuffer;
        outputMessageBuffer <<
            "---- Crash Report ----\n"
            "// " << _flavorTexts[std::rand() % _flavorTexts.size()] << "\n\n"
            "Time: " << timeBuffer << "\n"
            "Crashing Thread: " << Thread::get_Thread_Name(std::this_thread::get_id()) << "\n"
            "Description: " << message << "\n\n"
            "--- System Details ---\n" << SystemInfo::get_Consolidated_System_Info() << "\n\n"
            "---- Stack Trace -----\n" <<
#ifdef _GLIBCXX_HAS_STACKTRACE
            // NOTE: Waiting for MSYS2 to implement libstdc++_libbacktrace.a, or GCC to implement <stacktrace> fully
            std::to_string(std::stacktrace::current())
#else
            "G++ has not yet fully implemented stack traces defined in C++23. :)"
#endif
            << "\n"
        ;
        return outputMessageBuffer.str();
    }

    std::string _get_Crash_Path(std::tm* now) {
        if (!_crashPath.empty()) return _crashPath;
        
        char crashFileBuffer[] = "crash-YYYY-MM-DD_HH.mm.SS.txt";
        if (now != nullptr) {
            std::snprintf(crashFileBuffer, sizeof(crashFileBuffer), "crash-%04d-%02d-%02d_%02d.%02d.%02d.txt", // crash-2016-09-20_04.20.59.txt
                (now->tm_year + 1900), (now->tm_mon + 1), now->tm_mday, // Date
                now->tm_hour, now->tm_min, now->tm_sec // Time
            );
        }
        std::stringstream crashPath;
        crashPath << _crashDir << "/" << crashFileBuffer;
        return crashPath.str();
    }

    void _log_Crash_Report(const std::string& report, std::tm* now) {
        std::string crashPath = _get_Crash_Path(now);

        std::fputs(report.c_str(), stderr);
        try {
            FileIO::write_File(crashPath.c_str(), report);
        } catch (std::runtime_error &e) {
            std::fputs(e.what(), stderr);
        }
    }

    void _cleanup_and_Exit() {
        LoveEngineInstance::cleanup();
        exit(EXIT_FAILURE);
    }

    void _default_Crash_Function(const std::string& message) {
        std::tm* time;
        std::string crashReport = _generate_Crash_Report(message.empty() ? std::string(std::strerror(errno)) : message, time);
        _log_Crash_Report(crashReport, time);
    }

    [[noreturn]] void Crash::crash(const std::string& message) {
        if (!_crashed) {
            _crashed = true;
            _crashFunction(message);
            _cleanup_and_Exit();
        }
        throw std::runtime_error(message);
    }
}
