#include "lvcore/threading/thread_pool.hpp"

#include "lvcore/filesystem/filesystem.hpp"

#include "lvcore/core/instance.hpp"
#include "lvcore/core/device.hpp"
#include "lvcore/core/swap_chain.hpp"
#include "lvcore/core/pipeline_layout.hpp"
#include "lvcore/core/shader_module.hpp"
#include "lvcore/core/graphics_pipeline.hpp"
#include "lvcore/core/vertex_descriptor.hpp"
#include "lvcore/core/buffer.hpp"
#include "lvcore/core/descriptor_set.hpp"
#include "lvcore/core/image.hpp"
#include "lvcore/core/sampler.hpp"
#include "lvcore/core/render_pass.hpp"
#include "lvcore/core/framebuffer.hpp"
#include "lvcore/core/command_buffer.hpp"

#include "model.hpp"
#include "first_person_camera.hpp"

#include "application.hpp"

struct PCDeferredVP {
    glm::mat4 invViewProj;
    glm::vec3 viewPos;
};

struct MainRenderPass {
    lv::Subpass* gbufferSubpass;
    lv::Subpass* deferredSubpass;
    lv::RenderPass* renderPass;
    lv::Framebuffer* framebuffer;
    lv::CommandBuffer* commandBuffer;

    lv::Image* colorImage;
    lv::Image* diffuseImage;
    lv::Image* normalImage;
    lv::Image* depthImage;
#ifdef LV_BACKEND_METAL
    lv::Image* depthAsColorImage;
#endif
};


//TODO: destroy shader modules
class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool* threadPool;
    lv::Instance* instance;
    lv::Device* device;
    lv::SwapChain* swapChain;

    MainRenderPass mainRenderPass;

    lv::PipelineLayout* gbufferPipelineLayout;
    lv::PipelineLayout* deferredPipelineLayout;
    lv::PipelineLayout* hdrPipelineLayout;

    lv::ShaderModule* vertGBufferShaderModule;
    lv::ShaderModule* fragGBufferShaderModule;
    lv::GraphicsPipeline* gbufferGraphicsPipeline;

    lv::ShaderModule* vertTriangleShaderModule;
    lv::ShaderModule* fragDeferredShaderModule;
    lv::GraphicsPipeline* deferredGraphicsPipeline;

    lv::ShaderModule* fragHdrShaderModule;
    lv::GraphicsPipeline* hdrGraphicsPipeline;

    lv::VertexDescriptor* mainVertexDescriptor;
    
    lv::Buffer* uniformBuffer;

    lv::DescriptorSet* gbufferDescriptorSet;
    lv::DescriptorSet* deferredDescriptorSet;
    lv::DescriptorSet* hdrDescriptorSet;

    FirstPersonCamera camera;

    Model sponzaModel;

    float prevTime = 0.0f;

    LavaCoreExampleApp() : Application("tile_based_deferred_rendering") {
        threadPool = new lv::ThreadPool({});

        instance = new lv::Instance({
            .applicationName = exampleName.c_str(),
            .validationEnable = True
        });

        device = new lv::Device({
            .window = window,
            .threadPool = threadPool
        });

        swapChain = new lv::SwapChain({
            .window = window,
            .vsyncEnable = True,
            .maxFramesInFlight = 2
        });

        uint16_t framebufferWidth, framebufferHeight;
        lvndWindowGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        //Vertex descriptor
        mainVertexDescriptor = new lv::VertexDescriptor({
            .size = sizeof(MainVertex),
            .bindings = {
                {0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position)},
                {1, LV_VERTEX_FORMAT_RG32_SFLOAT, offsetof(MainVertex, texCoord)},
                {2, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, normal)}
            }
        });

        //Render pass
        mainRenderPass.colorImage = new lv::Image({
            .format = LV_FORMAT_R16G16B16A16_SFLOAT,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        mainRenderPass.diffuseImage = new lv::Image({
            .format = LV_FORMAT_R8G8B8A8_UNORM,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .memoryType = LV_MEMORY_TYPE_MEMORYLESS
        });

        mainRenderPass.normalImage = new lv::Image({
            .format = LV_FORMAT_R16G16B16A16_SNORM,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .memoryType = LV_MEMORY_TYPE_MEMORYLESS
        });

        mainRenderPass.depthImage = new lv::Image({
            .format = LV_FORMAT_D32_SFLOAT,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .aspectMask = LV_IMAGE_ASPECT_DEPTH_BIT
        });

#ifdef LV_BACKEND_METAL
        mainRenderPass.depthAsColorImage = new lv::Image({
            .format = LV_FORMAT_R32_SFLOAT,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .memoryType = LV_MEMORY_TYPE_MEMORYLESS
        });
#endif

        mainRenderPass.gbufferSubpass = new lv::Subpass({
            .colorAttachments = {
                {1, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {2, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
#ifdef LV_BACKEND_METAL
                {4, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
#endif
            },
            .depthAttachment = {3, LV_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}
        });

        mainRenderPass.deferredSubpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
            },
            .depthAttachment = {3, LV_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},
            .inputAttachments = {
                {1, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
                {2, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
#ifdef LV_BACKEND_VULKAN
                {3, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL}
#elif defined(LV_BACKEND_METAL)
                {4, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
#endif
            }
        });

        

        mainRenderPass.renderPass = new lv::RenderPass({
            .subpasses = {mainRenderPass.gbufferSubpass, mainRenderPass.deferredSubpass},
            .attachments = {
                {
                    .format = mainRenderPass.colorImage->format(),
                    .index = 0,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.diffuseImage->format(),
                    .index = 1,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.normalImage->format(),
                    .index = 2,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.depthImage->format(),
                    .index = 3,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .initialLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                    .finalLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                },
#ifdef LV_BACKEND_METAL
                {
                    .format = mainRenderPass.depthAsColorImage->format(),
                    .index = 4,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                }
#endif
            }
        });

        mainRenderPass.framebuffer = new lv::Framebuffer({
            .renderPass = mainRenderPass.renderPass,
            .colorAttachments = {
                {0, mainRenderPass.colorImage},
                {1, mainRenderPass.diffuseImage},
                {2, mainRenderPass.normalImage},
#ifdef LV_BACKEND_METAL
                {4, mainRenderPass.depthAsColorImage},
#endif
            },
            .depthAttachment = {3, mainRenderPass.depthImage}
        });

        mainRenderPass.commandBuffer = new lv::CommandBuffer({});

        //Pipeline layout

        //GBuffer
        gbufferPipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof(PCModel)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT}
                }},
                {{
                    {0, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //Deferred
        deferredPipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_FRAGMENT_BIT,
                    .offset = 0,
                    .size = sizeof(PCDeferredVP)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {1, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {2, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //HDR
        hdrPipelineLayout = new lv::PipelineLayout({
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //Graphics pipeline

        //GBuffer
        vertGBufferShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_VERTEX_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/vertex/gbuffer.json").c_str())
        });

        fragGBufferShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/gbuffer.json").c_str())
        });

        gbufferGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertGBufferShaderModule,
            .fragmentShaderModule = fragGBufferShaderModule,
            .pipelineLayout = gbufferPipelineLayout,
            .renderPass = mainRenderPass.renderPass,
            .vertexDescriptor = mainVertexDescriptor,
            .depthTestEnable = True,
            .cullMode = LV_CULL_MODE_BACK_BIT,
            .colorBlendAttachments = {
                {0},
                {1},
                {2},
#ifdef LV_BACKEND_METAL
                {4}
#endif
            }
        });

        //Deferred
        vertTriangleShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_VERTEX_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/vertex/triangle.json").c_str())
        });

        fragDeferredShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/deferred.json").c_str())
        });

        deferredGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragDeferredShaderModule,
            .pipelineLayout = deferredPipelineLayout,
            .renderPass = mainRenderPass.renderPass,
            .subpassIndex = 1,
            .depthTestEnable = True,
            .depthWriteEnable = False,
            .depthOp = LV_COMPARE_OP_NOT_EQUAL,
            .colorBlendAttachments = {
                {0},
                {1},
                {2},
#ifdef LV_BACKEND_METAL
                {4}
#endif
            }
        });

        //HDR
        fragHdrShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/hdr.json").c_str())
        });

        hdrGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragHdrShaderModule,
            .pipelineLayout = hdrPipelineLayout,
            .renderPass = swapChain->renderPass(),
            .colorBlendAttachments = {
                {0}
            }
        });

        //Uniform buffer
        uniformBuffer = new lv::Buffer({
            .size = sizeof(glm::mat4),
            .usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            .memoryType = LV_MEMORY_TYPE_SHARED
        });

        //Descriptor set
        gbufferDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = gbufferPipelineLayout,
            .layoutIndex = 0,
            .bufferBindings = {
                uniformBuffer->descriptorInfo(0)
            }
        });

        deferredDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = deferredPipelineLayout,
            .layoutIndex = 0,
            .imageBindings = {
                mainRenderPass.diffuseImage->descriptorInfo(0, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT),
                mainRenderPass.normalImage->descriptorInfo(1, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT),
#ifdef LV_BACKEND_VULKAN
                mainRenderPass.depthImage->descriptorInfo(2, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT),
#elif defined(LV_BACKEND_METAL)
                mainRenderPass.depthAsColorImage->descriptorInfo(2, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT),
#endif
            }
        });

        hdrDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = hdrPipelineLayout,
            .layoutIndex = 0,
            .imageBindings = {
                mainRenderPass.colorImage->descriptorInfo(0)
            }
        });

        //Camera
        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        camera.aspectRatio = (float)width / (float)height;

        //Copy commands
        lv::CommandBuffer* copyCommandBuffer = new lv::CommandBuffer({});
        copyCommandBuffer->beginRecording();
        copyCommandBuffer->beginBlitCommands();

        //Model
        sponzaModel.scale = glm::vec3(0.01f);
        sponzaModel.init(copyCommandBuffer, gbufferPipelineLayout, "../assets/models/sponza/scene.gltf", 1, 1);

        //Commit copy commands
        copyCommandBuffer->endRecording();
        copyCommandBuffer->submit();
        delete copyCommandBuffer;
    }

    ~LavaCoreExampleApp() {
        sponzaModel.destroy();

        delete gbufferDescriptorSet;
        delete deferredDescriptorSet;

        delete uniformBuffer;

        delete gbufferGraphicsPipeline;

        delete deferredGraphicsPipeline;

        delete gbufferPipelineLayout;
        delete deferredPipelineLayout;

        delete swapChain;
        delete device;
        delete instance;
    }

    void update() override {
        float crntTime = lvndGetTime();
        float dt = crntTime - prevTime;
        prevTime = crntTime;

        swapChain->acquireNextImage();

        //Rendering

        sponzaModel.calculateModel();

        //Main pass

        //GBuffer subpass
        mainRenderPass.commandBuffer->beginRecording();
        mainRenderPass.commandBuffer->beginRenderCommands(mainRenderPass.framebuffer);

        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(gbufferGraphicsPipeline);

        camera.inputs(window, dt);
        camera.loadViewProj();

        uniformBuffer->copyDataTo(&camera.viewProj);

        mainRenderPass.commandBuffer->cmdBindDescriptorSet(gbufferDescriptorSet);

        sponzaModel.render(mainRenderPass.commandBuffer);

        //Deferred subpass
        mainRenderPass.commandBuffer->cmdNextSubpass();

        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(deferredGraphicsPipeline);

        PCDeferredVP vp;
        vp.invViewProj = glm::inverse(camera.viewProj);
        vp.viewPos = camera.position;

        mainRenderPass.commandBuffer->cmdPushConstants(&vp, 0);

        mainRenderPass.commandBuffer->cmdBindDescriptorSet(deferredDescriptorSet);

        mainRenderPass.commandBuffer->cmdDraw(3);

        mainRenderPass.commandBuffer->endRecording();
        mainRenderPass.commandBuffer->submit();

        //HDR pass
        swapChain->commandBuffer()->beginRecording();
        swapChain->commandBuffer()->beginRenderCommands(swapChain->framebuffer());

        swapChain->commandBuffer()->cmdBindGraphicsPipeline(hdrGraphicsPipeline);

        swapChain->commandBuffer()->cmdBindDescriptorSet(hdrDescriptorSet);

        swapChain->commandBuffer()->cmdDraw(3);

        swapChain->commandBuffer()->endRecording();
        swapChain->renderAndPresent();
    }
};

int main() {
    LavaCoreExampleApp application;
    application.run();

    return 0;
}
