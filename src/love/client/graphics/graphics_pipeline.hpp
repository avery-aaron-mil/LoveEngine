#ifndef LOVE_GRAPHICS_PIPELINE_HPP
#define LOVE_GRAPHICS_PIPELINE_HPP

#include <memory>
#include <string>

#include <love/common/data/files/file_io.hpp>
#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class GraphicsPipeline {
        public:
            struct Shaders {
                std::string vertexShaderPath = "";
                std::string tessellationShaderPath = "";
                std::string fragmentShaderPath = "";
            };

            GraphicsPipeline(VkDevice device, const Shaders& shaders, std::shared_ptr<Logger> logger);
            ~GraphicsPipeline();

        private:
            std::shared_ptr<Logger> _logger;
            VkDevice _device;
            VkShaderModule _vertexShader = nullptr;
            VkShaderModule _tessellationShader = nullptr;
            VkShaderModule _fragmentShader = nullptr;

            void _log(const std::string& message) const noexcept;
            void _loadShaders(const Shaders& shaders) noexcept;
            [[nodiscard]] VkShaderModule _createShaderModule(const FileIO::FileContent& shaderCode) const noexcept;
    };
}

#endif // LOVE_GRAPHICS_PIPELINE_HPP
