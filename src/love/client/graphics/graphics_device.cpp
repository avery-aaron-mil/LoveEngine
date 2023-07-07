#include "graphics_device.hpp"

#include <sstream>
#include <unordered_set>

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    std::vector<const char*> _enabledExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void GraphicsDevice::_log(const std::string &message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void GraphicsDevice::addEnabledExtension(const char* extension) const noexcept {
        _enabledExtensions.push_back(extension);
    }

    void GraphicsDevice::_getPhysicalDevices() noexcept {
        _log("Getting physical graphics devices...");

        uint32_t deviceCount = 0;
        if (vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, nullptr) != VK_SUCCESS) {
            Crash::crash("Failed to enumerate physical devices.");
        }
        if (deviceCount == 0) {
            Crash::crash("Found no physical graphics devices.");
        }

        _physicalDevices.resize(deviceCount);
        if (vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, _physicalDevices.data()) != VK_SUCCESS) {
            Crash::crash("Failed to enumerate physical devices.");
        }

        for (size_t i = 0; i < deviceCount; i++) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(_physicalDevices[i], &properties);
            _physicalDevicesNameToIndex[properties.deviceName] = i;
        }

        std::stringstream buffer;
        buffer << "Got " << deviceCount << " physical devices.";
        _log(buffer.str());
    }

    GraphicsDevice::QueueFamilyIndices GraphicsDevice::_getDeviceQueueFamilies(const VkPhysicalDevice& device) const noexcept {
        QueueFamilyIndices queueIndices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        if (queueFamilyCount == 0) return queueIndices;

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, &queueFamilyProperties[0]);
        for (uint32_t i = 0; i < queueFamilyCount; ++i) {
            if (queueFamilyProperties[i].queueCount > 0) {
                // Check if queue can be used for graphics
                if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    queueIndices.hasGraphicsQueue = true;
                    queueIndices.graphicsQueue = i;
                }

                // Check if queue can be used for presenting
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
                if (presentSupport) {
                    queueIndices.hasPresentQueue = true;
                    queueIndices.presentQueue = i;
                }

                if (queueIndices.hasAllQueues()) break;
            }
        }

        return queueIndices;
    }
    
    bool GraphicsDevice::_checkDeviceHasEnabledExtensions(const VkPhysicalDevice& device) noexcept {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
            device,
            nullptr,
            &extensionCount,
            availableExtensions.data()
        );

        std::unordered_set<std::string> remainingExtensions(_enabledExtensions.begin(), _enabledExtensions.end());
        for (const auto &extension : remainingExtensions) {
            remainingExtensions.erase(extension);
        }

        return remainingExtensions.empty();
    }
    
    GraphicsDevice::SwapChainSupportDetails GraphicsDevice::_querySwapChainSupport(const VkPhysicalDevice& device) const noexcept {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

        if (formatCount > 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

        if (presentModeCount > 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device,
                _surface,
                &presentModeCount,
                details.presentModes.data()
            );
        }

        return details;
    }

    std::string GraphicsDevice::_getDeviceUnsuitabilityReason(const VkPhysicalDevice& device) const noexcept {
        // Check limits
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        if ((VK_VERSION_MAJOR(properties.apiVersion) < 1) && (properties.limits.maxImageDimension2D < 4096)) {
            return std::string("maxImageDimension2D < 4096.");
        }

        // Check features
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);
        if (!features.samplerAnisotropy) return std::string("Sampler anisotropy not supported.");
        if (!features.geometryShader) return std::string("Geometry shaders not supported.");
        if (!features.shaderInt64) return std::string("64-bit integers not supported.");
        if (!features.shaderFloat64) return std::string("64-bit floats not supported.");

        // Check queue families
        QueueFamilyIndices queueIndices = _getDeviceQueueFamilies(device);
        if (!queueIndices.hasAllQueues()) return std::string("No queues family with required queues found.");

        // Check extension support
        if (!_checkDeviceHasEnabledExtensions(device)) return std::string("Extensions not supported.");

        // Check swap chain support
        SwapChainSupportDetails swapChainSupport = _querySwapChainSupport(device);
        if (swapChainSupport.formats.empty()) return std::string("No swap chain formats supported.");
        if (swapChainSupport.presentModes.empty()) return std::string("No swap chain present modes supported.");

        // Device suitable; no reason to report
        return std::string();
    }

    int64_t GraphicsDevice::_rateDevice(const VkPhysicalDevice& device) const noexcept {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        // Fails if not suitable
        std::string excuse = _getDeviceUnsuitabilityReason(device);
        if (!excuse.empty()) {
            std::stringstream buffer;
            buffer << "Graphics device \"" << properties.deviceName << "\" failed rating: " << excuse;
            _log(buffer.str());
            return -1;
        }

        int64_t score = 0;

        // Prefer up-to-date
        score += VK_VERSION_MAJOR(properties.apiVersion) * 10000;
        score += VK_VERSION_MINOR(properties.apiVersion) * 100;
        score += VK_VERSION_PATCH(properties.apiVersion);

        // Discrete GPUs have a significant performance advantage
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000000000;

        // Maximum possible size of textures affects graphics quality
        score += properties.limits.maxImageDimension2D * 100; // Will be > 4096

        std::stringstream buffer;
        buffer << "Graphics device \"" << properties.deviceName << "\" passed with a rating of: " << score;
        _log(buffer.str());
        return score;
    }

    VkDevice GraphicsDevice::_createLogicalDevice(
        const VkPhysicalDevice& physicalDevice,
        const VkPhysicalDeviceFeatures& deviceFeatures
    ) const noexcept {
        _log("Creating Vulkan device...");

        // Input validation
        std::string excuse = _getDeviceUnsuitabilityReason(physicalDevice);
        if (!excuse.empty()) {
            std::stringstream buffer;
            buffer << "Could not create logical device because the physical device does not meet requirements: " << excuse;
            Crash::crash(buffer.str());
        }

        // Create queue create infos
        QueueFamilyIndices indices = _getDeviceQueueFamilies(physicalDevice);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::unordered_set<uint32_t> uniqueQueueFamilies = {indices.graphicsQueue, indices.presentQueue};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queueFamily,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,
            };
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Create device create info
        VkDeviceCreateInfo deviceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledExtensionCount = static_cast<uint32_t>(_enabledExtensions.size()),
            .ppEnabledExtensionNames = _enabledExtensions.data(),
            .pEnabledFeatures = &deviceFeatures
        };

        // Create device
        VkDevice device;
        if ((vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) ||
            (device == nullptr)
        ) {
            Crash::crash("Failed to create Vulkan device.");
        }

        _log("Created Vulkan device.");
        return device;
    }

    void GraphicsDevice::_loadDeviceVulkanFunctions() const noexcept {
        _log("Loading Vulkan device functions...");

        #define VK_DEVICE_LEVEL_FUNCTION(fun)\
        if (!(fun = (PFN_##fun) vkGetDeviceProcAddr(_device, #fun))) {\
            Crash::crash("Could not load Vulkan device function: " #fun);\
        }
        #include "vulkan_functions.inl"
        #undef VK_DEVICE_LEVEL_FUNCTION

        _log("Loaded Vulkan device functions.");
    }
    
    VkPhysicalDevice GraphicsDevice::getPhysicalDeviceFromName(const std::string& name) const {
        // Input validation
        if (_physicalDevices.empty()) throw std::invalid_argument("No physical graphics devices found.");
        if (!_physicalDevicesNameToIndex.contains(name)) {
            std::stringstream buffer;
            buffer << "No physical graphics device found containing device name: " << name;
            throw std::invalid_argument(buffer.str());
        }

        return _physicalDevices[_physicalDevicesNameToIndex.at(name)];
    }

    VkPhysicalDevice GraphicsDevice::getBestDevice() const {
        // Input validation
        if (_physicalDevices.empty()) throw std::invalid_argument("No physical graphics devices found.");

        int64_t bestScore = -1;
        size_t bestDeviceIndex = 0;
        int64_t deviceScore;
        for (size_t i = 0; i < _physicalDevices.size(); i++) {
            deviceScore = _rateDevice(_physicalDevices[i]);
            if (deviceScore > bestScore) {
                bestScore = deviceScore;
                bestDeviceIndex = i;
            }
        }

        return _physicalDevices[bestDeviceIndex];
    }

    void GraphicsDevice::usePhysicalDevice(const VkPhysicalDevice& device) noexcept {
        // Input validation
        if (device == nullptr) {
            Crash::crash("Null was passed to usePhysicalDevice().");
        }

        
        VkPhysicalDeviceFeatures deviceFeatures = {
            .samplerAnisotropy = VK_TRUE
        };
        _device = _createLogicalDevice(device, deviceFeatures);
        _loadDeviceVulkanFunctions();
    }

    void GraphicsDevice::_initGraphicsDevice(const std::string& deviceName) noexcept {
        _getPhysicalDevices();
        try {
            VkPhysicalDevice device = deviceName.empty() ? getBestDevice() : getPhysicalDeviceFromName(deviceName);
            usePhysicalDevice(device);
        } catch (std::invalid_argument& e) { Crash::crash(e.what()); }
    }

    GraphicsDevice::GraphicsDevice(std::shared_ptr<Logger> logger, VkInstance vulkanInstance, VkSurfaceKHR surface)
        : _logger(logger), _vulkanInstance(vulkanInstance), _surface(surface)
    {
        // Input validation
        if (_vulkanInstance == nullptr) Crash::crash("Vulkan instance passed to Graphics Device was null.");
        if (_surface == nullptr) Crash::crash("Surface passed to Graphics Device was null.");

        _initGraphicsDevice("");
    }
    GraphicsDevice::GraphicsDevice(
        std::shared_ptr<Logger> logger,
        VkInstance vulkanInstance,
        VkSurfaceKHR surface,
        const std::string& deviceName
    )
        : _logger(logger), _vulkanInstance(vulkanInstance), _surface(surface)
    {
        // Input validation
        if (_vulkanInstance == nullptr) Crash::crash("Vulkan instance passed to Graphics Device was null.");
        if (_surface == nullptr) Crash::crash("Surface passed to Graphics Device was null.");

        _initGraphicsDevice(deviceName);
    }

    GraphicsDevice::~GraphicsDevice() {
        if (_device) vkDestroyDevice(_device, nullptr);
    }
}