#ifndef LOVE_VULKAN_FUNCTIONS_HPP
#define LOVE_VULKAN_FUNCTIONS_HPP

#include "vulkan/vulkan.h"

namespace love_engine {

// Thanks to Ekzuky @ Intel for this idea of implementing functions in a natural-looking way
// https://www.intel.com/content/www/us/en/developer/articles/training/api-without-secrets-introduction-to-vulkan-part-1.html

#define VK_EXPORTED_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_GLOBAL_LEVEL_FUNCTION( fun) extern PFN_##fun fun;
#define VK_INSTANCE_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_DEVICE_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;

#include "vulkan_functions.inl"

}

#endif // LOVE_VULKAN_FUNCTIONS_HPP
