#ifndef LOVE_CLIENT_HPP
#define LOVE_CLIENT_HPP


#ifdef _WIN32
    #ifdef BUILDING_LOVE_CLIENT
        #define LV_ENG_CLI __declspec(dllexport)
    #else
        #define LV_ENG_CLI __declspec(dllimport)
    #endif
#else
    #define LV_ENG_CLI __attribute__((visibility("default")))
#endif

#endif // LOVE_CLIENT_HPP
