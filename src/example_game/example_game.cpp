#include <love/common/love_engine_instance.hpp>
#include <love/common/data/files/logger.hpp>
#include <love/common/system/system_info.hpp>

#include <love/client/client_instance.hpp>
#include <love/client/graphics/graphics_instance.hpp>

#include <example_game/client/client_states/client_state_loading.hpp>

#include <cstdlib>
#include <memory>

using namespace love_engine;
using namespace example_game;

std::shared_ptr<Logger> logger;

void glfwCallback(int e, const char*desc) {
    logger.get()->log("System Info:\n" + SystemInfo::getConsolidatedSystemInfo());
}

GraphicsInstance::Properties setGraphicsInstanceProperties() {
    // Set properties
    Window::Properties windowProperties;
    windowProperties.title = "Game";

    GraphicsPipeline::Shader vertexShader {
        .name = "Vertex Shader",
        .path = std::string(FileIO::getExecutableDirectory() + "../assets/shaders/vert.spv"),
        .stage = VK_SHADER_STAGE_VERTEX_BIT
    };
    GraphicsPipeline::Shader fragmentShader {
        .name = "Fragment Shader",
        .path = std::string(FileIO::getExecutableDirectory() + "../assets/shaders/frag.spv"),
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT
    };
    std::vector<GraphicsPipeline::Shader> shaders{vertexShader, fragmentShader};

    GraphicsPipeline::Properties pipelineProperties {
        .type = GraphicsPipeline::PipelineType::DEFAULT_3D,
        .shaders = std::move(shaders)
    };

    // Vulkan properties
    VulkanInstance::ApplicationInfo applicationInfo {
        .name = "Example Game",
        .versionMajor = 1,
        .versionMinor = 0,
        .versionPatch = 0,
        .debugLogger = logger,
        .verbose = true
    };
    VulkanInstance::Properties vulkanProperties {
        .windowProperties = std::move(windowProperties),
        .graphicsPipelineProperties = std::move(pipelineProperties),
        .glfwErrorCallback = glfwCallback
    };

    return GraphicsInstance::Properties{
        .applicationInfo = std::move(applicationInfo),
        .vulkanProperties = std::move(vulkanProperties)
    };
}

int main(int argc, char** argv) {
    // Initialize
    LoveEngineInstance loveEngine{FileIO::getExecutableDirectory() + "../crash-reports"};
    logger = std::make_shared<Logger>(Logger(FileIO::getExecutableDirectory() + "../logs/latest.log", true));
    logger.get()->log("System Info:\n" + SystemInfo::getConsolidatedSystemInfo());

    // Start client
    ClientState_Loading loadingState;
    ClientInstance::Properties clientInstanceProperties = {
        .graphicsProperties = setGraphicsInstanceProperties(),
        .msPerTick = 50.f
    };
    ClientInstance client(&loadingState, clientInstanceProperties, logger);
    client.run();

    logger.get()->log("Client exited. Closing.");
    loveEngine.cleanup();
    exit(EXIT_SUCCESS);
}
