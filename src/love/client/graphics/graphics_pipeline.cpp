#include "graphics_pipeline.hpp"

#include <sstream>
#include <vector>

#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    GraphicsPipeline::GraphicsPipeline(const Settings& settings)
    : _logger(settings.logger), _device(settings.device), _extent(settings.extent), _createInfo(settings.createInfo) {
        if (_device == nullptr) Crash::crash("Device passed to graphics pipeline was null.");
        _createPipeline();
    }
    GraphicsPipeline::~GraphicsPipeline() {
        for (auto shaderModule : _shaderModules) vkDestroyShaderModule(_device, shaderModule, nullptr);
        if (_pipelineLayout) vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
    }

    void GraphicsPipeline::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void GraphicsPipeline::_createPipeline() noexcept {
        if (_createInfo) {
            if (_createInfo->dynamicStateInfo == nullptr) {
                std::vector<VkDynamicState> dynamicStates = {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR
                };
                VkPipelineDynamicStateCreateInfo dynamicStateInfo {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                    .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
                    .pDynamicStates = dynamicStates.data() // TODO Would leak memory
                };
                _createInfo->dynamicStateInfo = std::make_shared<VkPipelineDynamicStateCreateInfo>(dynamicStateInfo);
            }

            /*if (_createInfo->vertexInputStateInfo == nullptr) {
                VkPipelineVertexInputStateCreateInfo vertexInputInfo {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
                };

                _createInfo->vertexInputStateInfo = std::make_shared<VkPipelineVertexInputStateCreateInfo>(vertexInputInfo);
            }
            
            if (_createInfo->inputAssemblyStateInfo == nullptr) {
                VkPipelineInputAssemblyStateCreateInfo inputAssembly {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
                };

                _createInfo->inputAssemblyStateInfo = std::make_shared<VkPipelineInputAssemblyStateCreateInfo>(inputAssembly);
            }

            if (_createInfo->viewportStateInfo == nullptr) {
                VkViewport viewport {
                    .x = 0.f,
                    .y = 0.f,
                    .width = static_cast<float>(_extent.width),
                    .height = static_cast<float>(_extent.height),
                    .minDepth = 0.f,
                    .maxDepth = 1.f
                };
                VkRect2D scissor {
                    .offset = {0, 0},
                    .extent = _extent
                };
                VkPipelineViewportStateCreateInfo viewportState {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                    .viewportCount = 1,
                    .pViewports = &viewport, // TODO Fix memory leak
                    .scissorCount = 1,
                    .pScissors = &scissor // TODO Fix memory leak
                };

                _createInfo->viewportStateInfo = std::make_shared<VkPipelineViewportStateCreateInfo>(viewportState);
            }

            if (_createInfo->rasterizationStateInfo == nullptr) {
                VkPipelineRasterizationStateCreateInfo rasterizer {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                    .polygonMode = VK_POLYGON_MODE_FILL,
                    .cullMode = VK_CULL_MODE_BACK_BIT,
                    .frontFace = VK_FRONT_FACE_CLOCKWISE,
                    .lineWidth = 1.f
                };

                _createInfo->rasterizationStateInfo = std::make_shared<VkPipelineRasterizationStateCreateInfo>(rasterizer);
            }

            if (_createInfo->multisampleStateInfo == nullptr) {
                VkPipelineMultisampleStateCreateInfo multisampling {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                    .minSampleShading = 1.f
                };

                _createInfo->multisampleStateInfo = std::make_shared<VkPipelineMultisampleStateCreateInfo>(multisampling);
            }

            if (_createInfo->colorBlendStateInfo == nullptr) {
                VkPipelineColorBlendAttachmentState colorBlendAttachment {
                    .blendEnable = VK_TRUE,
                    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                    .colorBlendOp = VK_BLEND_OP_ADD,
                    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .alphaBlendOp = VK_BLEND_OP_ADD,
                    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
                };
                VkPipelineColorBlendStateCreateInfo colorBlending {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                    .attachmentCount = 1,
                    .pAttachments = &colorBlendAttachment // TODO Fix memory leak
                };

                _createInfo->colorBlendStateInfo = std::make_shared<VkPipelineColorBlendStateCreateInfo>(colorBlending);
            }*/

            if (_createInfo->pipelineLayoutInfo == nullptr) {
                VkPipelineLayoutCreateInfo pipelineLayoutInfo {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
                };

                _createInfo->pipelineLayoutInfo = std::make_shared<VkPipelineLayoutCreateInfo>(pipelineLayoutInfo);
            }
        } else {
            _createInfo = std::make_shared<PipelineCreateInfo>();
        }

        if (vkCreatePipelineLayout(_device, _createInfo->pipelineLayoutInfo.get(), nullptr, &_pipelineLayout) != VK_SUCCESS) {
            Crash::crash("Failed to create pipeline layout.");
        }
    }

    void GraphicsPipeline::_loadShaders(const std::vector<Shader>& shaders) noexcept {
        _log("Loading shaders modules...");

        std::stringstream buffer;
        for (auto shader : shaders) {
            buffer.str();
            buffer << "Loading shader named: " << shader.name;
            _log(buffer.str());

            auto shaderCode = FileIO::readFileContent(shader.path);
            VkShaderModuleCreateInfo createInfo {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = shaderCode.size(),
                .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
            };

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                Crash::crash("Failed to create shader module.");
            }
            _shaderModules.emplace_back(std::move(shaderModule));

            VkPipelineShaderStageCreateInfo shaderStageInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = shader.stage,
                .module = _shaderModules.back(),
                .pName = shader.entryPoint.c_str()
            };
            _shaderStages[shader.stage] = std::move(shaderStageInfo);

            _log("Loaded shader.");
        }

        _log("Loaded shader modules.");
    }
}
