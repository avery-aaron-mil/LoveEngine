#ifndef LOVE_GRAPGICS_DEVICE_HPP
#define LOVE_GRAPGICS_DEVICE_HPP
#include <love/client/love_client.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class LV_ENG_CLI GraphicsDevice {
        public:
            struct Properties {
                std::string deviceName = "";
            };

            struct QueueFamilyIndices {
                bool hasGraphicsQueue = false;
                bool hasPresentQueue = false;
                uint32_t graphicsQueue;
                uint32_t presentQueue;

                inline bool hasAllQueues() const noexcept { return hasGraphicsQueue && hasPresentQueue; }
            };

            GraphicsDevice(VkInstance vulkanInstance, VkSurfaceKHR surface, const Properties& properties, std::shared_ptr<Logger> logger);
            ~GraphicsDevice();

            void addEnabledExtension(const char* extension) noexcept;
            VkPhysicalDevice getBestDevice() const;
            VkPhysicalDevice getPhysicalDeviceFromName(const std::string& name) const;
            void usePhysicalDevice(const VkPhysicalDevice& device) noexcept;

            inline VkDevice device() const noexcept { return _device; }
            inline VkPhysicalDevice physicalDevice() const noexcept { return _physicalDevice; }
            inline const QueueFamilyIndices& queueFamilyIndices() const noexcept { return _queueFamilyIndices; }
            inline std::string getDeviceName() const noexcept { return _properties.deviceName; }

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            Properties _properties;
            VkInstance _vulkanInstance = nullptr;
            VkSurfaceKHR _surface = nullptr;
            VkPhysicalDevice _physicalDevice = nullptr;
            VkDevice _device = nullptr;

            std::vector<VkPhysicalDevice> _physicalDevices;
            std::unordered_map<std::string, size_t> _physicalDevicesNameToIndex;
            QueueFamilyIndices _queueFamilyIndices;
            std::vector<const char*> _enabledExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

            void _log(const std::string& message) const noexcept;
            void _getPhysicalDevices() noexcept;
            QueueFamilyIndices _getDeviceQueueFamilyIndices(const VkPhysicalDevice& device) const noexcept;
            bool _checkDeviceHasEnabledExtensions(const VkPhysicalDevice& device) const noexcept;
            std::string _getDeviceUnsuitabilityReason(const VkPhysicalDevice& device) const noexcept;
            int64_t _rateDevice(const VkPhysicalDevice& device) const noexcept;
            VkDevice _createLogicalDevice(
                const VkPhysicalDevice& physicalDevice,
                const VkPhysicalDeviceFeatures& deviceFeatures
            ) const noexcept;
            void _loadDeviceVulkanFunctions() const noexcept;
            void _initDevice(const std::string& deviceName) noexcept;
    };
}

#endif // LOVE_GRAPGICS_DEVICE_HPP
