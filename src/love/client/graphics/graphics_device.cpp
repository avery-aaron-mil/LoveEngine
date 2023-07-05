#include "graphics_device.hpp"

#include <sstream>

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    void GraphicsDevice::_log(const std::string& message) const noexcept {
        if (_settings.logger.get() != nullptr) {
            _settings.logger.get()->log(message);
        }
    }

    void GraphicsDevice::_get_Physical_Devices() noexcept {
        _log("Getting physical graphics devices...");

        uint32_t deviceCount = 0;
        if (vkEnumeratePhysicalDevices(Vulkan.Instance, &deviceCount, nullptr) != VK_SUCCESS) {
            Crash::crash("Failed to enumerate physical devices.");
        }
        if (deviceCount == 0) {
            Crash::crash("Found no physical graphics devices.");
        }

        _physicalDevices.resize(deviceCount);
        if (vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, _physicalDevices.data()) != VK_SUCCESS ) {
            Crash::crash("Failed to enumerate physical devices.");
        }

        std::stringstream buffer;
        buffer << "Got " << deviceCount << " physical devices.";
        _log(buffer.str());
    }

    std::string _check_Device_Suitability(
        const VkPhysicalDevice& device,
        const VkPhysicalDeviceProperties& properties,
        const VkPhysicalDeviceFeatures& features
    ) noexcept {
        if ((VK_VERSION_MAJOR(properties.apiVersion) < 1) &&
            (properties.limits.maxImageDimension2D < 4096)
        ) return std::string("maxImageDimension2D < 4096");

        // TODO
        QueueFamilyIndices queueFamilyIndicies = findQueueFamilies(device);
        if (!queueFamilyIndicies.is_Complete()) {
            return std::string("Queue family indicies are not complete.");
        }

        if (!checkDeviceExtensionSupport(device)) return std::string("Extensions not supported.");

        // TODO
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        if (swapChainSupport.formats.empty()) {
            return std::string("Swap Chain formats support is empty.");
        }
        if (!swapChainSupport.presentModes.empty()) {
            return std::string("Swap Chain presentModes support is empty.");
        }

        if (!features.samplerAnisotropy) {
            return std::string("Sampler anisotropy not supported.");
        }

        if (!features.geometryShader) {
            return std::string("Geometry shaders not supported.");
        }
        
        if (!features.shaderInt64) {
            return std::string("64-bit integers not supported.");
        }
        
        if (!features.shaderFloat64) {
            return std::string("64-bit floats not supported.");
        }

        return std::string();
    }

    int64_t GraphicsDevice::_rate_Device(const VkPhysicalDevice& device) noexcept {
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;

        vkGetPhysicalDeviceProperties(device, &properties);
        vkGetPhysicalDeviceFeatures(device, &features);

        uint32_t versionMajor = VK_VERSION_MAJOR(properties.apiVersion);
        uint32_t versionMinor = VK_VERSION_MINOR(properties.apiVersion);
        uint32_t versionPatch = VK_VERSION_PATCH(properties.apiVersion);

        // Fails if not suitable
        std::string excuse = _check_Device_Suitability(device, properties, features);
        if (!excuse.empty()) {
            std::stringstream buffer;
            buffer << "Graphics device \"" << properties.deviceName << "\" failed rating: " << excuse;
            _log(buffer.str());
            return -1;
        }

        // Rate
        int64_t score = 0;

        // Prefer up-to-date
        score += 100 * versionMajor;
        score += 10 * versionMinor;
        score += versionPatch;
        
        // Discrete GPUs have a significant performance advantage
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000000;

        // Maximum possible size of textures affects graphics quality
        score += 10 * properties.limits.maxImageDimension2D;

        std::stringstream buffer;
        buffer << "Graphics device \"" << properties.deviceName << "\" passed with a rating of: " << score;
        _log(buffer.str());
        return score;
    }

    void GraphicsDevice::_create_Device() noexcept {
        _log("Creating Vulkan device...");
        //
        _log("Created Vulkan device.");
    }

    void GraphicsDevice::_load_Device_Vulkan_Functions() const noexcept {
        _log("Loading Vulkan device functions...");
        #define VK_GLOBAL_LEVEL_FUNCTION(fun)\
        if(!(fun = (PFN_##fun) vkGetDeviceProcAddr(_device, #fun))) {\
            Crash::crash("Could not load Vulkan device function: " #fun);\
        }
        #include "vulkan_functions.inl"
        _log("Loaded Vulkan device functions.");
    }

    GraphicsDevice::GraphicsDevice(std::shared_ptr<Logger> logger, VkInstance vulkanInstance)
        : _logger(logger), _vulkanInstance(vulkanInstance)
    {
        _get_Physical_Devices();
        _create_Device();
        _load_Device_Vulkan_Functions();
    }

    GraphicsDevice::~GraphicsDevice() {
        if (_device) vkDestroyDevice(_device, nullptr);
    }
}
