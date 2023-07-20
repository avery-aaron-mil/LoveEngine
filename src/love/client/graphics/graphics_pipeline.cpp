#include "graphics_pipeline.hpp"

#include <sstream>
#include <vector>

#include <love/common/data/files/file_io.hpp>
#include <love/common/error/crash.hpp>

#include "vulkan_functions.hpp"

namespace love_engine {
    GraphicsPipeline::GraphicsPipeline(
        VkDevice device,
        VkExtent2D extent,
        const Properties& properties,
        std::shared_ptr<Logger> logger
    ) : _logger(logger), _device(device), _extent(extent), _properties(properties) {
        if (_device == nullptr) Crash::crash("Device passed to graphics pipeline was null.");
        _loadShaders();
        _preparePipeline();
        _createPipeline();
    }
    GraphicsPipeline::~GraphicsPipeline() {
        for (auto shaderModule : _shaderModules) vkDestroyShaderModule(_device, shaderModule, nullptr);
        if (_pipelineLayout) vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        if (_pipeline) vkDestroyPipeline(_device, _pipeline, nullptr);
    }

    void GraphicsPipeline::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log(message);
        }
    }

    void GraphicsPipeline::_loadShaders() noexcept {
        _log("Loading shaders modules...");

        if (_properties.shaders.empty()) Crash::crash("No shaders passed to loadShaders().");

        std::stringstream buffer;
        for (auto shader : _properties.shaders) {
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
            _shaderModules.push_back(shaderModule);

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

    void GraphicsPipeline::_preparePipeline() noexcept {
        _log("Preparing graphics pipeline...");
        if (_properties.createInfo) {
            if (_properties.createInfo->dynamicStateInfo.get() == nullptr) {
                if (_properties.dynamicStates.empty()) {
                    _properties.dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
                    _properties.dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
                }

                VkPipelineDynamicStateCreateInfo dynamicStateInfo {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                    .dynamicStateCount = static_cast<uint32_t>(_properties.dynamicStates.size()),
                    .pDynamicStates = _properties.dynamicStates.data()
                };
                _properties.createInfo->dynamicStateInfo = std::make_shared<VkPipelineDynamicStateCreateInfo>(dynamicStateInfo);
            }

            if (_properties.createInfo->vertexInputStateInfo.get() == nullptr) {
                VkPipelineVertexInputStateCreateInfo vertexInputInfo {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
                };

                _properties.createInfo->vertexInputStateInfo =
                    std::make_shared<VkPipelineVertexInputStateCreateInfo>(vertexInputInfo);
            }
            
            if (_properties.createInfo->inputAssemblyStateInfo.get() == nullptr) {
                VkPipelineInputAssemblyStateCreateInfo inputAssembly {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
                };

                _properties.createInfo->inputAssemblyStateInfo =
                    std::make_shared<VkPipelineInputAssemblyStateCreateInfo>(inputAssembly);
            }

            if (_properties.createInfo->viewportStateInfo.get() == nullptr) {
                if (_properties.viewports.empty()) {
                    VkViewport viewport {
                        .x = 0.f,
                        .y = 0.f,
                        .width = static_cast<float>(_extent.width),
                        .height = static_cast<float>(_extent.height),
                        .minDepth = 0.f,
                        .maxDepth = 1.f
                    };

                    _properties.viewports.emplace_back(std::move(viewport));
                }

                if (_properties.scissors.empty()) {
                    VkRect2D scissor {
                        .offset = {0, 0},
                        .extent = _extent
                    };

                    _properties.scissors.emplace_back(std::move(scissor));
                }

                VkPipelineViewportStateCreateInfo viewportState {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                    .viewportCount = static_cast<uint32_t>(_properties.viewports.size()),
                    .pViewports = _properties.viewports.data(),
                    .scissorCount = static_cast<uint32_t>(_properties.scissors.size()),
                    .pScissors = _properties.scissors.data()
                };

                _properties.createInfo->viewportStateInfo = std::make_shared<VkPipelineViewportStateCreateInfo>(viewportState);
            }

            if (_properties.createInfo->rasterizationStateInfo.get() == nullptr) {
                VkPipelineRasterizationStateCreateInfo rasterizer {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                    .polygonMode = VK_POLYGON_MODE_FILL,
                    .cullMode = VK_CULL_MODE_BACK_BIT,
                    .frontFace = VK_FRONT_FACE_CLOCKWISE,
                    .lineWidth = 1.f
                };

                _properties.createInfo->rasterizationStateInfo =
                    std::make_shared<VkPipelineRasterizationStateCreateInfo>(rasterizer);
            }

            if (_properties.createInfo->multisampleStateInfo.get() == nullptr) {
                VkPipelineMultisampleStateCreateInfo multisampling {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                    .minSampleShading = 1.f
                };

                _properties.createInfo->multisampleStateInfo =
                    std::make_shared<VkPipelineMultisampleStateCreateInfo>(multisampling);
            }

            if (_properties.createInfo->colorBlendStateInfo.get() == nullptr) {
                if (_properties.createInfo->colorBlendAttachments.empty()) {
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

                    _properties.createInfo->colorBlendAttachments.emplace_back(std::move(colorBlendAttachment));
                }

                VkPipelineColorBlendStateCreateInfo colorBlending {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                    .attachmentCount = static_cast<uint32_t>(_properties.createInfo->colorBlendAttachments.size()),
                    .pAttachments = _properties.createInfo->colorBlendAttachments.data()
                };

                _properties.createInfo->colorBlendStateInfo =
                    std::make_shared<VkPipelineColorBlendStateCreateInfo>(colorBlending);
            }

            if (_properties.createInfo->pipelineLayoutInfo.get() == nullptr) {
                VkPipelineLayoutCreateInfo pipelineLayoutInfo {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
                };

                _properties.createInfo->pipelineLayoutInfo = std::make_shared<VkPipelineLayoutCreateInfo>(pipelineLayoutInfo);
            }
        } else {
            _properties.createInfo = std::make_shared<PipelineCreateInfo>();
        }

        if (vkCreatePipelineLayout(
                _device, _properties.createInfo->pipelineLayoutInfo.get(), nullptr, &_pipelineLayout
            ) != VK_SUCCESS
        ) {
            Crash::crash("Failed to create pipeline layout.");
        }

        _log("Prepared graphics pipeline.");
    }

    void GraphicsPipeline::_createPipeline() noexcept {
        _log("Creating graphics pipeline...");

        VkGraphicsPipelineCreateInfo pipelineInfo {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = static_cast<uint32_t>(_shaderStages.size()),
            .pStages = _shaderStages.data(),
            .pVertexInputState = _properties.createInfo->vertexInputStateInfo.get(),
            .pInputAssemblyState = _properties.createInfo->inputAssemblyStateInfo.get(),
            .pViewportState = _properties.createInfo->viewportStateInfo.get(),
            .pRasterizationState = _properties.createInfo->rasterizationStateInfo.get(),
            .pMultisampleState = _properties.createInfo->multisampleStateInfo.get(),
            .pDepthStencilState = _properties.createInfo->depthStencilStateInfo.get(),
            .pColorBlendState = _properties.createInfo->colorBlendStateInfo.get(),
            .pDynamicState = _properties.createInfo->dynamicStateInfo.get(),
            .layout = _pipelineLayout,
            .renderPass = _renderPass.get()->renderPass(),
            .subpass = 0,
            .basePipelineHandle = nullptr,
            .basePipelineIndex = -1
        };

        if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS) {
            Crash::crash("Failed to create graphics pipeline.");
        }

        _log("Created graphics pipeline.");
    }
}
