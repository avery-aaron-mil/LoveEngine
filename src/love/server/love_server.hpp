#ifndef LOVE_SERVER_HPP
#define LOVE_SERVER_HPP


#ifdef _WIN32
    #ifdef BUILDING_LOVE_SERVER
        #define LV_ENG_SER __declspec(dllexport)
    #else
        #define LV_ENG_SER __declspec(dllimport)
    #endif
#else
    #define LV_ENG_SER __attribute__((visibility("default")))
#endif

#endif // LOVE_SERVER_HPP
