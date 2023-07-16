#ifndef LOVE_SHADERS_HPP
#define LOVE_SHADERS_HPP

#include <memory>
#include <string>
#include <vector>

#include <love/common/data/files/file_io.hpp>
#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class Shaders {
        public:
            struct Shader {
                std::string name = "Unknown";
                std::string path = "";
                std::string entryPoint = "main";
                VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT;
            };

            Shaders(VkDevice device, const std::vector<Shader>& shaders, std::shared_ptr<Logger> logger);
            ~Shaders();

            static constexpr VkShaderStageFlagBits VERTEX_SHADER = VK_SHADER_STAGE_VERTEX_BIT;
            static constexpr VkShaderStageFlagBits TESSELLATION_SHADER = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            static constexpr VkShaderStageFlagBits FRAGMENT_SHADER = VK_SHADER_STAGE_FRAGMENT_BIT;

        private:
            std::shared_ptr<Logger> _logger;
            VkDevice _device;
            std::vector<VkShaderModule> _shaderModules;
            std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;

            void _log(const std::string& message) const noexcept;
            void _loadShaders(const std::vector<Shader>& shaders) noexcept;
            [[nodiscard]] VkShaderModule _createShaderModule(const FileIO::FileContent& shaderCode) const noexcept;
    };
}

#endif // LOVE_SHADERS_HPP
