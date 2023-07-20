#ifndef LOVE_GRAPHICS_PIPELINE_HPP
#define LOVE_GRAPHICS_PIPELINE_HPP

#include <memory>
#include <string>
#include <vector>

#include "render_pass.hpp"

#include <love/common/data/files/file_io.hpp>
#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class GraphicsPipeline {
        public:
            enum class PipelineType {
                CUSTOM = 0,
                DEFAULT_2D,
                DEFAULT_3D,
                MONOCHROME_2D,
                MONOCHROME_3D,
                CELL_SHADED_2D,
                CELL_SHADED_3D,
            };
            struct Shader {
                std::string name = "Unknown"; // The name of the shader
                std::shared_ptr<FileIO::FileContent> data = nullptr; // The loaded code of the shader. Loads from path if this is null.
                std::string path = ""; // The path to load the shader code from.
                std::string entryPoint = "main"; // The name of the entry point function in the shader
                VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT; // The Vulkan stage of the shader (vertex, fragment, compute, etc.)
            };
            struct PipelineCreateInfo {
                std::shared_ptr<VkPipelineShaderStageCreateInfo> shaderStageInfo = nullptr;
                std::shared_ptr<VkPipelineDynamicStateCreateInfo> dynamicStateInfo = nullptr;
                std::shared_ptr<VkPipelineVertexInputStateCreateInfo> vertexInputStateInfo = nullptr;
                std::shared_ptr<VkPipelineInputAssemblyStateCreateInfo> inputAssemblyStateInfo = nullptr;
                std::shared_ptr<VkPipelineViewportStateCreateInfo> viewportStateInfo = nullptr;
                std::shared_ptr<VkPipelineRasterizationStateCreateInfo> rasterizationStateInfo = nullptr;
                std::shared_ptr<VkPipelineMultisampleStateCreateInfo> multisampleStateInfo = nullptr;
                std::shared_ptr<VkPipelineDepthStencilStateCreateInfo> depthStencilStateInfo = nullptr;
                std::shared_ptr<VkPipelineColorBlendStateCreateInfo> colorBlendStateInfo = nullptr;
                std::shared_ptr<VkPipelineLayoutCreateInfo> pipelineLayoutInfo = nullptr;
            };
            struct Properties {
                PipelineType type = PipelineType::DEFAULT_3D;
                std::shared_ptr<PipelineCreateInfo> createInfo = nullptr;
                std::vector<Shader> shaders;
                std::vector<VkDynamicState> dynamicStates;
                std::vector<VkViewport> viewports;
                std::vector<VkRect2D> scissors;
                std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
            };

            GraphicsPipeline(
                VkDevice device,
                VkExtent2D extent,
                const RenderPass& renderPass,
                const Properties& properties,
                std::shared_ptr<Logger> logger
            );
            ~GraphicsPipeline();

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            VkDevice _device = nullptr;
            VkExtent2D _extent;
            RenderPass _renderPass;
            Properties _properties;
            VkPipelineLayout _pipelineLayout = nullptr;
            std::vector<VkShaderModule> _shaderModules;
            std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
            VkPipeline _pipeline = nullptr;

            void _log(const std::string& message) const noexcept;
            void _loadDefaultProperties() noexcept;
            void _validatePipelineCreateInfo() noexcept;
            void _createPipelineLayout() noexcept;
            void _createPipeline() noexcept;
            void _loadShaders() noexcept;

            void _loadPropertiesDefault2D() noexcept;
            void _loadPropertiesDefault3D() noexcept;
            void _loadPropertiesMonochrome2D() noexcept;
            void _loadPropertiesMonochrome3D() noexcept;
            void _loadPropertiesCellShaded2D() noexcept;
            void _loadPropertiesCellShaded3D() noexcept;
    };
}

#endif // LOVE_GRAPHICS_PIPELINE_HPP
