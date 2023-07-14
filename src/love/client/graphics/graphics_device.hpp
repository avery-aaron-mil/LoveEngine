#ifndef LOVE_GRAPHICS_DEVICE_HPP
#define LOVE_GRAPHICS_DEVICE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

#include "graphics_instance.hpp"

namespace love_engine {
    class GraphicsDevice {
        public:
            struct Settings {
                std::string deviceName = "";
                VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            };

            struct QueueFamilyIndices {
                bool hasGraphicsQueue = false;
                bool hasPresentQueue = false;
                uint32_t graphicsQueue;
                uint32_t presentQueue;

                inline bool hasAllQueues() const noexcept { return hasGraphicsQueue && hasPresentQueue; }
            };
            struct SwapChainSupportDetails {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> surfaceFormats;
                std::vector<VkPresentModeKHR> presentModes;
            };

            GraphicsDevice(VkInstance vulkanInstance, VkSurfaceKHR surface, const Settings& settings, std::shared_ptr<Logger> logger);
            ~GraphicsDevice();

            void addEnabledExtension(const char* extension) noexcept;
            VkPhysicalDevice getBestDevice() const;
            VkPhysicalDevice getPhysicalDeviceFromName(const std::string& name) const;
            void usePhysicalDevice(const VkPhysicalDevice& device) noexcept;

            inline VkDevice device() const noexcept { return _device; }
            inline std::string getDeviceName() const noexcept { return _settings.deviceName; }

        private:
            std::shared_ptr<Logger> _logger;
            Settings _settings;
            VkInstance _vulkanInstance;
            VkDevice _device;
            VkSurfaceKHR _surface;

            std::vector<VkPhysicalDevice> _physicalDevices;
            std::unordered_map<std::string, size_t> _physicalDevicesNameToIndex;
            QueueFamilyIndices _queueFamilyIndices;
            SwapChainSupportDetails _swapChainSupport;
            std::vector<const char*> _enabledExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

            VkQueue _graphicsQueue;
            VkQueue _presentQueue;

            void _log(const std::string& message) const noexcept;
            void _getPhysicalDevices() noexcept;
            QueueFamilyIndices _getDeviceQueueFamilyIndices(const VkPhysicalDevice& device) const noexcept;
            bool _checkDeviceHasEnabledExtensions(const VkPhysicalDevice& device) const noexcept;
            SwapChainSupportDetails _querySwapChainSupport(const VkPhysicalDevice& device) const noexcept;
            std::string _getDeviceUnsuitabilityReason(const VkPhysicalDevice& device) const noexcept;
            int64_t _rateDevice(const VkPhysicalDevice& device) const noexcept;
            VkDevice _createLogicalDevice(
                const VkPhysicalDevice& physicalDevice,
                const VkPhysicalDeviceFeatures& deviceFeatures
            ) const noexcept;
            void _loadDeviceVulkanFunctions() const noexcept;
            void _initGraphicsDevice(const std::string& deviceName) noexcept;
    };
}

#endif // LOVE_GRAPHICS_DEVICE_HPP
