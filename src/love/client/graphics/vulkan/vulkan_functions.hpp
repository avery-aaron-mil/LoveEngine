#ifndef LOVE_VULKAN_FUNCTIONS_HPP
#define LOVE_VULKAN_FUNCTIONS_HPP

#include <vulkan/vulkan.h>
#if defined (_WIN32)
  #ifndef VK_USE_PLATFORM_WIN32_KHR
  #define VK_USE_PLATFORM_WIN32_KHR
  #endif
  #include <windows.h>
  #include <vulkan/vulkan_win32.h>
#elif defined(VK_USE_PLATFORM_XCB_KHR)
  #ifndef VK_USE_PLATFORM_XCB_KHR
  #define VK_USE_PLATFORM_XCB_KHR
  #endif
  #include <xbc/proto.h>
  #include <vulkan/vulkan_xcb.h>
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
  #ifndef VK_USE_PLATFORM_XLIB_KHR
  #define VK_USE_PLATFORM_XLIB_KHR
  #endif
  #include <X11/Xlib.h>
  #include <vulkan/vulkan_xlib.h>
#elif defined(__ANDROID__)
  #ifndef VK_USE_PLATFORM_ANDROID_KHR
  #define VK_USE_PLATFORM_ANDROID_KHR
  #endif
  #include <vulkan/vulkan_android.h>
#endif

namespace love_engine {

// Thanks to Ekzuky @ Intel for this idea of implementing functions in a natural-looking way
// https://www.intel.com/content/www/us/en/developer/articles/training/api-without-secrets-introduction-to-vulkan-part-1.html

#define VK_EXPORTED_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_GLOBAL_LEVEL_FUNCTION( fun) extern PFN_##fun fun;
#define VK_INSTANCE_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_DEVICE_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;

#include <love/client/graphics/vulkan/vulkan_functions.inl>

}

#endif // LOVE_VULKAN_FUNCTIONS_HPP
