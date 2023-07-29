#ifndef LOVE_COMMON_HPP
#define LOVE_COMMON_HPP


#ifdef _WIN32
    #ifdef BUILDING_LOVE_COMMON
        #define LV_ENG_COM __declspec(dllexport)
    #else
        #define LV_ENG_COM __declspec(dllimport)
    #endif
#else
    #define LV_ENG_COM __attribute__((visibility("default")))
#endif

#endif // LOVE_COMMON_HPP
