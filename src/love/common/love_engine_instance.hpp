#ifndef LOVE_LOVE_ENGINE_INSTANCE_HPP
#define LOVE_LOVE_ENGINE_INSTANCE_HPP

namespace love_engine {
    class LoveEngineInstance {
        public:
            static void init() { init(FileIO::get_Executable_Directory() + "crash-reports/"); }
            static void init(const std::string& crashDirectory);
            static void cleanup();
    };
}

#endif // LOVE_LOVE_ENGINE_INSTANCE_HPP
