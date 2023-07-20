#include "graphics_pipeline.hpp"

#include <sstream>
#include <vector>

#include <love/common/data/files/file_io.hpp>
#include <love/common/error/crash.hpp>

#include <vulkan/vk_enum_string_helper.h>

#include "vulkan_functions.hpp"

namespace love_engine {
    GraphicsPipeline::GraphicsPipeline(
        VkDevice device,
        VkExtent2D extent,
        const RenderPass& renderPass,
        const Properties& properties,
        std::shared_ptr<Logger> logger
    ) : _logger(logger), _device(device), _extent(extent), _renderPass(renderPass), _properties(properties) {
        if (_device == nullptr) Crash::crash("Device passed to graphics pipeline was null.");
        if ((_properties.type != PipelineType::CUSTOM) || (_properties.createInfo.get() == nullptr)) {
            _loadDefaultProperties();
        } 
        _loadShaders();
        _validatePipelineCreateInfo();
        _createPipelineLayout();
        _createPipeline();
    }
    GraphicsPipeline::~GraphicsPipeline() { 
        for (auto shaderModule : _shaderModules) vkDestroyShaderModule(_device, shaderModule, nullptr);
        if (_pipelineLayout) vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        if (_pipeline) vkDestroyPipeline(_device, _pipeline, nullptr);
    }

    void GraphicsPipeline::_log(const std::string& message) const noexcept {
        if (_logger.get() != nullptr) {
            _logger.get()->log("(Love/GraphicsPipeline): " + message);
        }
    }
    
    void GraphicsPipeline::_loadDefaultProperties() noexcept {
        switch (_properties.type) {
            case PipelineType::DEFAULT_2D: {
                _loadPropertiesDefault2D();
            } break;
            case PipelineType::DEFAULT_3D: {
                _loadPropertiesDefault3D();
            } break;
            case PipelineType::MONOCHROME_2D: {
                _loadPropertiesMonochrome2D();
            } break;
            case PipelineType::MONOCHROME_3D: {
                _loadPropertiesMonochrome3D();
            } break;
            case PipelineType::CELL_SHADED_2D: {
                _loadPropertiesCellShaded2D();
            } break;
            case PipelineType::CELL_SHADED_3D: {
                _loadPropertiesCellShaded3D();
            } break;
            default: {
                Crash::crash("Graphics pipeline type was not from the enum.");
            } break;
        }
    } 

    void GraphicsPipeline::_loadShaders() noexcept {
        _log("Loading shaders modules...");

        if (_properties.shaders.empty()) Crash::crash("No shaders passed to loadShaders().");

        std::stringstream buffer;
        for (size_t i = 0; i < _properties.shaders.size(); ++i) {
            buffer.str("");
            buffer << "Loading shader: " << _properties.shaders[i].name;
            _log(buffer.str());

            if (_properties.shaders[i].data.get() == nullptr) {
                try {
                    _properties.shaders[i].data = std::make_shared<FileIO::FileContent>(FileIO::readFileContent(_properties.shaders[i].path));
                } catch (std::runtime_error& e) {
                    Crash::crash(std::string("FileIO::readFileContent() failed to read shader data:\n\t") + e.what());
                }
            }
            VkShaderModuleCreateInfo createInfo {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = _properties.shaders[i].data.get()->size(),
                .pCode = reinterpret_cast<const uint32_t*>(_properties.shaders[i].data.get()->data())
            };

            VkShaderModule shaderModule;
            auto result = vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule);
            if (result != VK_SUCCESS) {
                std::stringstream crashBuffer;
                crashBuffer << "Failed to create shader module: " << string_VkResult(result);
                Crash::crash(crashBuffer.str());
            }
            _shaderModules.push_back(shaderModule);

            VkPipelineShaderStageCreateInfo shaderStageInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = _properties.shaders[i].stage,
                .module = _shaderModules.back(),
                .pName = _properties.shaders[i].entryPoint.c_str()
            };
            _shaderStages.emplace_back(std::move(shaderStageInfo));

            buffer.str("");
            buffer << "Loaded shader: " << _properties.shaders[i].name;
            _log(buffer.str());
        }

        _log("Loaded shader modules.");
    }

    void GraphicsPipeline::_validatePipelineCreateInfo() noexcept { // TODO Just load from default property loaders
        _log("Validating graphics pipeline create info...");

        if (_properties.createInfo.get() == nullptr) {
            _properties.createInfo = std::make_shared<PipelineCreateInfo>();
        }

        if (_properties.createInfo.get()->dynamicStateInfo.get() == nullptr) {
            if (_properties.dynamicStates.empty()) {
                _properties.dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
                _properties.dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
            }

            VkPipelineDynamicStateCreateInfo dynamicStateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .dynamicStateCount = static_cast<uint32_t>(_properties.dynamicStates.size()),
                .pDynamicStates = _properties.dynamicStates.data()
            };
            _properties.createInfo.get()->dynamicStateInfo =
                std::make_shared<VkPipelineDynamicStateCreateInfo>(dynamicStateInfo);
        }

        if (_properties.createInfo.get()->vertexInputStateInfo.get() == nullptr) {
            VkPipelineVertexInputStateCreateInfo vertexInputInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
            };

            _properties.createInfo.get()->vertexInputStateInfo =
                std::make_shared<VkPipelineVertexInputStateCreateInfo>(vertexInputInfo);
        }
        
        if (_properties.createInfo.get()->inputAssemblyStateInfo.get() == nullptr) {
            VkPipelineInputAssemblyStateCreateInfo inputAssembly {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
            };

            _properties.createInfo.get()->inputAssemblyStateInfo =
                std::make_shared<VkPipelineInputAssemblyStateCreateInfo>(inputAssembly);
        }

        if (_properties.createInfo.get()->viewportStateInfo.get() == nullptr) {
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

            _properties.createInfo.get()->viewportStateInfo =
                std::make_shared<VkPipelineViewportStateCreateInfo>(viewportState);
        }

        if (_properties.createInfo.get()->rasterizationStateInfo.get() == nullptr) {
            VkPipelineRasterizationStateCreateInfo rasterizer {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .polygonMode = VK_POLYGON_MODE_FILL,
                .cullMode = VK_CULL_MODE_BACK_BIT,
                .frontFace = VK_FRONT_FACE_CLOCKWISE,
                .lineWidth = 1.f
            };

            _properties.createInfo.get()->rasterizationStateInfo =
                std::make_shared<VkPipelineRasterizationStateCreateInfo>(rasterizer);
        }

        if (_properties.createInfo.get()->multisampleStateInfo.get() == nullptr) {
            VkPipelineMultisampleStateCreateInfo multisampling {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                .minSampleShading = 1.f
            };

            _properties.createInfo.get()->multisampleStateInfo =
                std::make_shared<VkPipelineMultisampleStateCreateInfo>(multisampling);
        }

        if (_properties.createInfo.get()->colorBlendStateInfo.get() == nullptr) {
            if (_properties.colorBlendAttachments.empty()) {
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

                _properties.colorBlendAttachments.emplace_back(std::move(colorBlendAttachment));
            }

            VkPipelineColorBlendStateCreateInfo colorBlending {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .attachmentCount = static_cast<uint32_t>(_properties.colorBlendAttachments.size()),
                .pAttachments = _properties.colorBlendAttachments.data()
            };

            _properties.createInfo.get()->colorBlendStateInfo =
                std::make_shared<VkPipelineColorBlendStateCreateInfo>(colorBlending);
        }

        if (_properties.createInfo.get()->pipelineLayoutInfo.get() == nullptr) {
            VkPipelineLayoutCreateInfo pipelineLayoutInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
            };

            _properties.createInfo.get()->pipelineLayoutInfo =
                std::make_shared<VkPipelineLayoutCreateInfo>(pipelineLayoutInfo);
        }

        _log("Validated graphics pipeline create info.");
    }
    
    void GraphicsPipeline::_createPipelineLayout() noexcept {
        _log("Creating graphics pipeline layout...");

        if (_properties.createInfo.get()->pipelineLayoutInfo.get() == nullptr) Crash::crash("Pipeline layout info was null.");

        auto result = vkCreatePipelineLayout(
            _device, _properties.createInfo.get()->pipelineLayoutInfo.get(), nullptr, &_pipelineLayout
        );
        if (result != VK_SUCCESS) {
            std::stringstream buffer;
            buffer << "Failed to create pipeline layout: " << string_VkResult(result);
            Crash::crash(buffer.str());
        }

        _log("Created graphics pipeline layout."); 
    }

    void GraphicsPipeline::_createPipeline() noexcept {
        _log("Creating graphics pipeline...");

        if (_renderPass.renderPass() == nullptr) Crash::crash("Render pass was null.");
        if (_properties.createInfo.get() == nullptr) Crash::crash("Create info struct was null.");

        const PipelineCreateInfo* createInfo = _properties.createInfo.get();
        VkGraphicsPipelineCreateInfo pipelineInfo {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = static_cast<uint32_t>(_shaderStages.size()),
            .pStages = _shaderStages.data(),
            .pVertexInputState = createInfo->vertexInputStateInfo.get(),
            .pInputAssemblyState = createInfo->inputAssemblyStateInfo.get(),
            .pViewportState = createInfo->viewportStateInfo.get(),
            .pRasterizationState = createInfo->rasterizationStateInfo.get(),
            .pMultisampleState = createInfo->multisampleStateInfo.get(),
            .pDepthStencilState = createInfo->depthStencilStateInfo.get(),
            .pColorBlendState = createInfo->colorBlendStateInfo.get(),
            .pDynamicState = createInfo->dynamicStateInfo.get(),
            .layout = _pipelineLayout,
            .renderPass = _renderPass.renderPass(),
            .subpass = _properties.subpassIndex,
            .basePipelineHandle = _properties.basePipeline,
            .basePipelineIndex = _properties.basePipelineIndex
        };

        auto result = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline);
        if (result != VK_SUCCESS) {
            std::stringstream buffer;
            buffer << "Failed to create graphics pipeline: " << string_VkResult(result);
            Crash::crash(buffer.str());
        }

        _log("Created graphics pipeline.");
    }
    
    void GraphicsPipeline::_loadPropertiesDefault2D() noexcept {
        // TODO
    }
    
    void GraphicsPipeline::_loadPropertiesDefault3D() noexcept {
        // TODO
    }
    
    void GraphicsPipeline::_loadPropertiesMonochrome2D() noexcept {
        // TODO
    }
    
    void GraphicsPipeline::_loadPropertiesMonochrome3D() noexcept {
        // TODO
    }
    
    void GraphicsPipeline::_loadPropertiesCellShaded2D() noexcept {
        // TODO
    }
    
    void GraphicsPipeline::_loadPropertiesCellShaded3D() noexcept {
        // TODO
    }
}
