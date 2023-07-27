#include <love/client/graphics/vulkan/graphics_device.hpp>

#include <cstring>
#include <sstream>
#include <unordered_set>

#include <love/client/graphics/vulkan/swap_chain.hpp>
#include <love/client/graphics/vulkan/vulkan_functions.hpp>
#include <love/common/error/crash.hpp>

#include <vulkan/vk_enum_string_helper.h>
#include <GLFW/glfw3.h>

namespace love_engine {
    GraphicsDevice::GraphicsDevice(
        VkInstance vulkanInstance,
        VkSurfaceKHR surface,
        const Properties& properties,
        std::shared_ptr<Logger> logger
    )
        : _logger(logger), _properties(properties), _vulkanInstance(vulkanInstance), _surface(surface)
    {
        // Input validation
        if (_vulkanInstance == nullptr) Crash::crash("Vulkan instance passed to Graphics GraphicsDevice was null.");
        if (_surface == nullptr) Crash::crash("Surface passed to Graphics GraphicsDevice was null.");

        _initDevice(_properties.deviceName);
    }

    GraphicsDevice::~GraphicsDevice() {
        if (_device) vkDestroyDevice(_device, nullptr);
        _log("Destroyed graphics device.");
    }

    void GraphicsDevice::_initDevice(const std::string& deviceName) noexcept {
        _getPhysicalDevices();
        try {
            VkPhysicalDevice device = deviceName.empty() ? getBestDevice() : getPhysicalDeviceFromName(deviceName);
            usePhysicalDevice(device);
        } catch (std::invalid_argument& e) { Crash::crash(e.what()); }
    }

    void GraphicsDevice::_log(const std::string &message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/GraphicsDevice): " + message);
        }
    }

    void GraphicsDevice::addEnabledExtension(const char* extension) noexcept {
        _enabledExtensions.push_back(extension);
    }

    void GraphicsDevice::_getPhysicalDevices() noexcept {
        _log("Getting physical graphics devices...");

        uint32_t deviceCount = 0;
        auto result = vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, nullptr);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to enumerate physical graphics devices: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }
        if (deviceCount == 0) {
            Crash::crash("Found no physical graphics devices.");
        }

        _physicalDevices.resize(deviceCount);
        result = vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, _physicalDevices.data());
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to enumerate physical graphics devices: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        for (size_t i = 0; i < deviceCount; ++i) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(_physicalDevices[i], &properties);
            _physicalDevicesNameToIndex[properties.deviceName] = i;
        }

        std::stringstream buffer;
        buffer << "Got " << deviceCount << " physical graphics devices.";
        _log(buffer.str());
    }

    GraphicsDevice::QueueFamilyIndices GraphicsDevice::_getDeviceQueueFamilyIndices(const VkPhysicalDevice& device) const noexcept {
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
                auto result = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
                if (result != VK_SUCCESS) {
                    std::stringstream crashBuffer;
                    crashBuffer << "Failed to get surface support of physical graphics device: " << string_VkResult(result);
                    Crash::crash(crashBuffer.str());
                }
                if (presentSupport) {
                    queueIndices.hasPresentQueue = true;
                    queueIndices.presentQueue = i;
                }

                if (queueIndices.hasAllQueues()) break;
            }
        }

        return queueIndices;
    }
    
    bool GraphicsDevice::_checkDeviceHasEnabledExtensions(const VkPhysicalDevice& device) const noexcept {
        uint32_t extensionCount;
        auto result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to enumerate device extension properites for physical graphics device: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        // Get available extensions
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to enumerate device extension properites for physical graphics device: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
        }

        // Search extensions
        for (const auto& extension : _enabledExtensions) {
            bool extensionFound = false;

            for (size_t i = 0; i < availableExtensions.size(); i++) {
                if (std::strcmp(extension, availableExtensions[i].extensionName) == 0) {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound) {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(device, &properties);
                std::stringstream buffer;
                buffer << "Could not find extension \"" << extension << "\" for device \"" << properties.deviceName << "\".";
                _log(buffer.str());
                return false;
            }
        }

        return true;
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

        // Check queue families
        QueueFamilyIndices queueIndices = _getDeviceQueueFamilyIndices(device);
        if (!queueIndices.hasAllQueues()) return std::string("No queues family with required queues found.");

        // Check extension support
        if (!_checkDeviceHasEnabledExtensions(device)) return std::string("Extensions not supported.");

        // Check swap chain support
        auto swapChainSupport = SwapChain::querySwapChainSupport(device, _surface);
        if (swapChainSupport.surfaceFormats.empty()) return std::string("No swap chain formats supported.");
        if (swapChainSupport.presentModes.empty()) return std::string("No swap chain present modes supported.");

        // GraphicsDevice suitable; no reason to report
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
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);
            std::stringstream buffer;
            buffer << "Could not create logical device because the physical graphics device \"" << properties.deviceName
                << "\" does not meet requirements: " << excuse
            ;
            Crash::crash(buffer.str());
        }

        // Create queue create infos
        QueueFamilyIndices indices = _getDeviceQueueFamilyIndices(physicalDevice);
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
        auto result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
        if (result != VK_SUCCESS) {
            std::stringstream crashBuffer;
            crashBuffer << "Failed to create Vulkan device: " << string_VkResult(result);
            Crash::crash(crashBuffer.str());
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
        #include <love/client/graphics/vulkan/vulkan_functions.inl>
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
        for (size_t i = 0; i < _physicalDevices.size(); ++i) {
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
        if (device == nullptr) Crash::crash("Vulkan device to usePhysicalDevice() was null.");

        // Log
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        _properties.deviceName = properties.deviceName; // Don't forget to set used device name for users!
        std::stringstream buffer;
        buffer << "Using physical device: " << properties.deviceName;
        _log(buffer.str());
        _physicalDevice = device;

        // Create logical device
        VkPhysicalDeviceFeatures deviceFeatures = {
            .samplerAnisotropy = VK_TRUE
        };
        if (_device) vkDestroyDevice(_device, nullptr);
        _device = _createLogicalDevice(device, deviceFeatures);
        _loadDeviceVulkanFunctions();

        // Prepare SwapChain
        _queueFamilyIndices = _getDeviceQueueFamilyIndices(device);
        if (!glfwGetPhysicalDevicePresentationSupport(_vulkanInstance, device, _queueFamilyIndices.presentQueue)) {
            Crash::crash("No presentation support for GLFW.");
        }
    }
}
