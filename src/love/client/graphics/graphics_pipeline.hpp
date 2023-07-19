#ifndef LOVE_GRAPHICS_PIPELINE_HPP
#define LOVE_GRAPHICS_PIPELINE_HPP

#include <memory>
#include <string>
#include <vector>

#include "render_pass.hpp"

#include <love/common/data/files/logger.hpp>

#include <vulkan/vulkan.h>

namespace love_engine {
    class GraphicsPipeline {
        public:
            struct Shader {
                std::string name = "Unknown";
                std::string path = "";
                std::string entryPoint = "main";
                VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT;
            };
            struct PipelineCreateInfo { // TODO Make an enum for each default pipeline type, with functions that set the default settings for each one of these infos.
                std::vector<Shader> shaders;
                std::shared_ptr<VkPipelineShaderStageCreateInfo> shaderStageInfo = nullptr;
                std::vector<VkDynamicState> dynamicStates;
                std::shared_ptr<VkPipelineDynamicStateCreateInfo> dynamicStateInfo = nullptr;
                std::shared_ptr<VkPipelineVertexInputStateCreateInfo> vertexInputStateInfo = nullptr;
                std::shared_ptr<VkPipelineInputAssemblyStateCreateInfo> inputAssemblyStateInfo = nullptr;
                std::vector<VkViewport> viewports;
                std::vector<VkRect2D> scissors;
                std::shared_ptr<VkPipelineViewportStateCreateInfo> viewportStateInfo = nullptr;
                std::shared_ptr<VkPipelineRasterizationStateCreateInfo> rasterizationStateInfo = nullptr;
                std::shared_ptr<VkPipelineMultisampleStateCreateInfo> multisampleStateInfo = nullptr;
                std::shared_ptr<VkPipelineDepthStencilStateCreateInfo> depthStencilStateInfo = nullptr;
                std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
                std::shared_ptr<VkPipelineColorBlendStateCreateInfo> colorBlendStateInfo = nullptr;
                std::shared_ptr<VkPipelineLayoutCreateInfo> pipelineLayoutInfo = nullptr;
            };
            struct Settings {
                VkDevice device = nullptr;
                VkExtent2D extent = {};
                std::shared_ptr<PipelineCreateInfo> createInfo = {};
                std::shared_ptr<Logger> logger;
            };

            GraphicsPipeline(const Settings& settings);
            ~GraphicsPipeline();

        private:
            std::shared_ptr<Logger> _logger = nullptr;
            VkDevice _device = nullptr;
            VkExtent2D _extent;
            std::shared_ptr<PipelineCreateInfo> _createInfo = nullptr;
            VkPipelineLayout _pipelineLayout = nullptr;
            std::vector<VkShaderModule> _shaderModules;
            std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
            std::unique_ptr<RenderPass> _renderPass;
            VkPipeline _pipeline = nullptr;

            void _log(const std::string& message) const noexcept;
            void _preparePipeline() noexcept;
            void _createPipeline() noexcept;
            void _loadShaders() noexcept;
    };
}

#endif // LOVE_GRAPHICS_PIPELINE_HPP
