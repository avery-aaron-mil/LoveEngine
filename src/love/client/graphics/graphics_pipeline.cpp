#include "graphics_pipeline.hpp"

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    GraphicsPipeline::GraphicsPipeline(VkDevice device, const Shaders& shaders, std::shared_ptr<Logger> logger)
    : _logger(logger), _device(device) {
        _loadShaders(shaders);
    }
    GraphicsPipeline::~GraphicsPipeline() {
        if (_vertexShader) vkDestroyShaderModule(_device, _vertexShader, nullptr);
    }

    void GraphicsPipeline::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void GraphicsPipeline::_loadShaders(const Shaders& shaders) noexcept {
        _log("Loading shaders...");
        if (!shaders.vertexShaderPath.empty()) {
            _log("Loading vertex shader...");
            auto vertexShaderCode = FileIO::readFileContent(shaders.vertexShaderPath);
            _vertexShader = _createShaderModule(vertexShaderCode);
            _log("Loaded vertex shader.");
        }
        if (!shaders.fragmentShaderPath.empty()) {
            _log("Loading fragment shader...");
            auto fragmentShaderCode = FileIO::readFileContent(shaders.fragmentShaderPath);
            _fragmentShader = _createShaderModule(fragmentShaderCode);
            _log("Loaded fragment shader.");
        }
        if (!shaders.tessellationShaderPath.empty()) {
            _log("Loading tessellation shader...");
            auto tessellationShaderCode = FileIO::readFileContent(shaders.tessellationShaderPath);
            _tessellationShader = _createShaderModule(tessellationShaderCode);
            _log("Loaded tessellation shader.");
        }
        _log("Loaded shaders.");
    }

    [[nodiscard]] VkShaderModule GraphicsPipeline::_createShaderModule(const FileIO::FileContent& shaderCode) const noexcept {
        VkShaderModuleCreateInfo createInfo{
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
