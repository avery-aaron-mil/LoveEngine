#ifndef LOVE_GRAPHICS_DEVICE_HPP
#define LOVE_GRAPHICS_DEVICE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h> // Must be included before GLFW

#include "graphics_instance.hpp"

namespace love_engine {
    class GraphicsDevice {
        public:
            struct QueueFamilyIndices {
                bool hasGraphicsQueue = false;
                bool hasPresentQueue = false;
                uint32_t graphicsQueue;
                uint32_t presentQueue;

                inline bool hasAllQueues() const noexcept { return hasGraphicsQueue && hasPresentQueue; }
            };
            struct SwapChainSupportDetails {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> presentModes;
            };

            GraphicsDevice(std::shared_ptr<Logger> logger, VkInstance vulkanInstance, VkSurfaceKHR surface);
            GraphicsDevice(
                std::shared_ptr<Logger> logger,
                VkInstance vulkanInstance,
                VkSurfaceKHR surface,
                const std::string& deviceName
            );
            ~GraphicsDevice();

            void addEnabledExtension(const char* extension) const noexcept;
            VkPhysicalDevice getBestDevice() const;
            VkPhysicalDevice getPhysicalDeviceFromName(const std::string& name) const;
            void usePhysicalDevice(const VkPhysicalDevice& device) noexcept;

        private:
            std::shared_ptr<Logger> _logger;
            VkInstance _vulkanInstance;
            VkDevice _device;
            VkSurfaceKHR _surface;
            std::vector<VkPhysicalDevice> _physicalDevices;
            std::unordered_map<std::string, size_t> _physicalDevicesNameToIndex;

            void _log(const std::string& message) const noexcept;
            void _getPhysicalDevices() noexcept;
            QueueFamilyIndices _getDeviceQueueFamilies(const VkPhysicalDevice& device) const noexcept;
            static bool _checkDeviceHasEnabledExtensions(const VkPhysicalDevice& device) noexcept;
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