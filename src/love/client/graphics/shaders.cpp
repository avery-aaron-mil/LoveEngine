#include "shaders.hpp"

#include <love/common/error/crash.hpp>

#include <sstream>

#include "vulkan_functions.hpp"

namespace love_engine {
    Shaders::Shaders(VkDevice device, const std::vector<Shader>& shaders, std::shared_ptr<Logger> logger)
    : _logger(logger), _device(device) {
        _loadShaders(shaders);
    }
    Shaders::~Shaders() {
        for (auto shaderModule : _shaderModules) vkDestroyShaderModule(_device, shaderModule, nullptr);
    }

    void Shaders::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void Shaders::_loadShaders(const std::vector<Shader>& shaders) noexcept {
        _log("Loading shaders modules...");

        std::stringstream buffer;
        for (auto shader : shaders) {
            buffer.str();
            buffer << "Loading shader named: " << shader.name;
            _log(buffer.str());

            auto shaderCode = FileIO::readFileContent(shader.path);
            _shaderModules.emplace_back(_createShaderModule(shaderCode));
            VkPipelineShaderStageCreateInfo shaderStageInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = shader.stage,
                .module = _shaderModules.back(),
                .pName = shader.entryPoint.c_str()
            };
            _shaderStages.emplace_back(std::move(shaderStageInfo));

            _log("Loaded shader.");
        }

        _log("Loaded shader modules.");
    }

    [[nodiscard]] VkShaderModule Shaders::_createShaderModule(const FileIO::FileContent& shaderCode) const noexcept {
        VkShaderModuleCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = shaderCode.size(),
            .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
        };

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            Crash::crash("Failed to create shader module.");
        }
        
        return shaderModule;
    }
}
