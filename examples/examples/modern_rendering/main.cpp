#include <random>

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

#define SKYLIGHT_IMAGE_SIZE 1024

#define SHADOW_MAP_SIZE 1024
#define SHADOW_CASCADE_COUNT 3
#define SHADOW_FAR_PLANE 48.0f

//#define SSR_MIP_COUNT 5

#define AO_NOISE_TEX_SIZE 8
#define SSAO_SAMPLE_SET_COUNT 16
#define SSAO_KERNEL_SIZE 8

enum AOType {
    AO_TYPE_SSAO,
    AO_TYPE_HBAO
};

#define AO_TYPE AO_TYPE_SSAO

float lerp(float a, float b, float f) {
  	return a + f * (b - a);
}

//Halton
float createHaltonSequence(unsigned int index, int base) {
    float f = 1;
    float r = 0;
    int current = index;
    do {
        f = f / base;
        r = r + f * (current % base);
        current = glm::floor(current / base);
    } while (current > 0);
    return r;
}

struct PCSkylightVP {
    glm::mat4 view;
    int32_t layerIndex;
};

struct PCDeferredVP {
    glm::mat4 invViewProj;
    glm::mat4 shadowViewProjs[SHADOW_CASCADE_COUNT];
    glm::vec3 viewPos;
};

struct PCSkyboxVP {
    glm::mat4 viewProj;
    glm::mat4 prevViewProj;
};

struct PCSsrVP {
    glm::mat4 viewProj;
    glm::mat4 invViewProj;
    glm::mat4 view;
    glm::vec3 viewPos;
    uint32_t frameIndex;
};

struct PCSsaoVP {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 invViewProj;
    uint32_t frameIndex;
};

struct PCLightShaftVP {
    glm::mat4 viewProj;
    glm::mat4 invViewProj;
    glm::mat4 shadowViewProjs[SHADOW_CASCADE_COUNT];
    glm::vec3 viewPos;
};

struct PCTemporalVP {
    //glm::mat4 projection;
    //glm::mat4 prevInvProj;
    glm::mat4 viewProj;
    glm::mat4 invViewProj;
    //glm::mat4 prevInvViewProj;
    //glm::mat4 invView;
    //glm::mat4 invProj;
    //glm::mat4 prevView;
    glm::vec4 lightColor;
    glm::vec3 viewPos;
    float reflectionTemporalDivider;
    uint32_t frameIndex;
    uint32_t useSpatialFilter;
};

struct UBOLight {
    glm::vec4 color; //TODO: fix alignment
    glm::vec4 direction;
};

struct UBOGBufferVP {
    glm::mat4 viewProj;
    glm::mat4 prevViewProj;
    uint32_t frameIndex;
};

struct SimpleRenderPass {
    lv::Subpass* subpass;
    lv::RenderPass* renderPass;
    lv::Framebuffer* framebuffer;
    lv::CommandBuffer* commandBuffer;

    lv::Image* colorImage;
};

struct ShadowRenderPass {
    lv::Subpass* subpass;
    lv::RenderPass* renderPass;
    lv::Framebuffer* framebuffer;
    lv::CommandBuffer* commandBuffer;

    lv::Image* depthImage;
    lv::Sampler* depthSampler;
};

struct MainRenderPass {
    lv::Subpass* gbufferSubpass;
    lv::Subpass* deferredSubpass;
    lv::Subpass* skyboxSubpass;
    lv::RenderPass* renderPass;
    lv::Framebuffer* framebuffer;
    lv::CommandBuffer* commandBuffer;

    lv::Image* colorImage;
    lv::Image* albedoMetallicImage;
    lv::Image* normalRoughnessImage;
    lv::Image* motionImage;
    lv::Image* f0Image;
    lv::Image* depthImage;
#ifdef LV_BACKEND_METAL
    lv::Image* depthAsColorImage;
#endif
    lv::Image* halfDepthImage;
};

struct BlurRenderPass {
    lv::Subpass* subpass;
    lv::RenderPass* renderPass;
    lv::CommandBuffer* commandBuffer;

    lv::Framebuffer* lightShaftBlurFramebuffer;

    lv::Image* lightShaftBlurImage;
};

struct CompositeRenderPass {
    lv::Subpass* ssaoSubpass;
    lv::Subpass* temporalResolveSubpass;
    lv::RenderPass* renderPass;
    lv::Framebuffer* framebuffer;
    lv::CommandBuffer* commandBuffer;

    lv::Image* ssaoImage;
    lv::Image* resolvedSsrImage;
    lv::Image* resolvedSsaoImage;
    lv::Image* resolvedColorImage;
    lv::Image* finalColorImage;
};

//TODO: delete shader modules after creating pipeline
class LavaCoreExampleApp : public Application {
public:
    lv::ThreadPool* threadPool;
    lv::Instance* instance;
    lv::Device* device;
    lv::SwapChain* swapChain;

    ShadowRenderPass shadowRenderPass;
    MainRenderPass mainRenderPass;
    SimpleRenderPass ssrRenderPass;
    SimpleRenderPass lightShaftRenderPass;
    BlurRenderPass blurRenderPass;
    CompositeRenderPass compositeRenderPass;
    SimpleRenderPass motionBlurRenderPass;

    lv::PipelineLayout* shadowPipelineLayout;
    lv::PipelineLayout* gbufferPipelineLayout;
    lv::PipelineLayout* deferredPipelineLayout;
    lv::PipelineLayout* skyboxPipelineLayout;
    lv::PipelineLayout* ssrPipelineLayout;
    lv::PipelineLayout* ssaoPipelineLayout;
    lv::PipelineLayout* lightShaftPipelineLayout;
    lv::PipelineLayout* blurPipelineLayout;
    lv::PipelineLayout* temporalResolvePipelineLayout;
    lv::PipelineLayout* motionBlurPipelineLayout;
    lv::PipelineLayout* hdrPipelineLayout;

    lv::ShaderModule* vertShadowShaderModule;
    lv::ShaderModule* fragShadowShaderModule;
    lv::GraphicsPipeline* shadowGraphicsPipeline;

    lv::ShaderModule* vertGBufferShaderModule;
    lv::ShaderModule* fragGBufferShaderModule;
    lv::GraphicsPipeline* gbufferGraphicsPipeline;

    lv::ShaderModule* vertTriangleShaderModule;
    lv::ShaderModule* fragDeferredShaderModule;
    lv::GraphicsPipeline* deferredGraphicsPipeline;

    lv::ShaderModule* vertSkyboxShaderModule;
    lv::ShaderModule* fragSkyboxShaderModule;
    lv::GraphicsPipeline* skyboxGraphicsPipeline;

    lv::ShaderModule* fragSsrShaderModule;
    lv::GraphicsPipeline* ssrGraphicsPipeline;

    lv::ShaderModule* fragSsaoShaderModule;
    lv::GraphicsPipeline* ssaoGraphicsPipeline;

    lv::ShaderModule* fragLightShaftShaderModule;
    lv::GraphicsPipeline* lightShaftGraphicsPipeline;

    lv::ShaderModule* fragBlurShaderModule;
    lv::GraphicsPipeline* blurGraphicsPipeline;

    lv::ShaderModule* fragTemporalResolveShaderModule;
    lv::GraphicsPipeline* temporalResolveGraphicsPipeline;

    lv::ShaderModule* fragMotionBlurShaderModule;
    lv::GraphicsPipeline* motionBlurGraphicsPipeline;

    lv::ShaderModule* fragHdrShaderModule;
    lv::GraphicsPipeline* hdrGraphicsPipeline;

    lv::VertexDescriptor* shadowVertexDescriptor;
    lv::VertexDescriptor* mainVertexDescriptor;
    lv::VertexDescriptor* skyboxVertexDescriptor;
    
    lv::Buffer* shadowUniformBuffer;//s[SHADOW_CASCADE_COUNT];
    lv::Buffer* gbufferUniformBuffer;
    lv::Buffer* lightUniformBuffer;

    lv::Buffer* skyboxVertexBuffer;
    lv::Buffer* skyboxIndexBuffer;

    lv::Image* aoNoiseImage;
    lv::Image* brdfLUTImage;
    lv::Image* skylightImage;

    lv::Sampler* basicSampler;
    lv::Sampler* repeatSampler;

    lv::DescriptorSet* shadowDescriptorSet;//s[SHADOW_CASCADE_COUNT];
    lv::DescriptorSet* gbufferDescriptorSet;
    lv::DescriptorSet* deferredDescriptorSet;
    lv::DescriptorSet* skyboxDescriptorSet;
    lv::DescriptorSet* ssrDescriptorSet;
    lv::DescriptorSet* ssaoDescriptorSet;
    lv::DescriptorSet* lightShaftDescriptorSet;
    lv::DescriptorSet* lightShaftBlurDescriptorSet;
    lv::DescriptorSet* temporalResolveDescriptorSet1;
    lv::DescriptorSet* temporalResolveDescriptorSet2;
    lv::DescriptorSet* temporalResolveDescriptorSet3;
    lv::DescriptorSet* motionBlurDescriptorSet;
    lv::DescriptorSet* hdrDescriptorSet;

    FirstPersonCamera camera;

    Model sponzaModel;
    //Model backpackModel;

    float prevTime = 0.0f;
    uint32_t frameIndex = 0;

    //Shadows
    const float cascadeLevels[SHADOW_CASCADE_COUNT] = {
        SHADOW_FAR_PLANE * 0.08f, SHADOW_FAR_PLANE * 0.32f, SHADOW_FAR_PLANE
    };

    PCDeferredVP deferredVP;
    UBOLight light;

    LavaCoreExampleApp() : Application("modern_rendering") {
        threadPool = new lv::ThreadPool({});

        instance = new lv::Instance({
            .applicationName = exampleName.c_str(),
            .validationEnable = lv::True
        });

        device = new lv::Device({
            .window = window,
            .threadPool = threadPool
        });

        swapChain = new lv::SwapChain({
            .window = window,
            .maxFramesInFlight = 2
        });

        uint16_t framebufferWidth, framebufferHeight;
        lvndWindowGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        //Vertex descriptor
        shadowVertexDescriptor = new lv::VertexDescriptor({
            .size = sizeof(MainVertex),
            .bindings = {
                {0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position)}
            }
        });

        mainVertexDescriptor = new lv::VertexDescriptor({
            .size = sizeof(MainVertex),
            .bindings = {
                {0, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, position)},
                {1, LV_VERTEX_FORMAT_RG32_SFLOAT, offsetof(MainVertex, texCoord)},
                {2, LV_VERTEX_FORMAT_RGB32_SFLOAT, offsetof(MainVertex, normal)},
                {3, LV_VERTEX_FORMAT_RGBA32_SFLOAT, offsetof(MainVertex, tangent)}
            }
        });

        skyboxVertexDescriptor = new lv::VertexDescriptor({
            .size = sizeof(glm::vec3),
            .bindings = {
                {0, LV_VERTEX_FORMAT_RGB32_SFLOAT, 0}
            }
        });

        //Render passes

        basicSampler = new lv::Sampler({});
        repeatSampler = new lv::Sampler({
            .addressMode = LV_SAMPLER_ADDRESS_MODE_REPEAT
        });

        //Shadow
        shadowRenderPass.depthImage = new lv::Image({
            .format = lv::Format::D32Float,
            .width = SHADOW_MAP_SIZE,
            .height = SHADOW_MAP_SIZE,
            .layerCount = SHADOW_CASCADE_COUNT,
            .imageType = lv::ImageType::_2DArray,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .aspectMask = LV_IMAGE_ASPECT_DEPTH_BIT
        });
        shadowRenderPass.depthSampler = new lv::Sampler({
            .filter = lv::Filter::Linear,
            .compareEnable = lv::True,
            .compareOp = LV_COMPARE_OP_LESS
        });

        shadowRenderPass.subpass = new lv::Subpass({
            .depthAttachment = {0, LV_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}
        });

        shadowRenderPass.renderPass = new lv::RenderPass({
            .subpasses = {shadowRenderPass.subpass},
            .attachments = {
                {
                    .index = 0,
                    .format = shadowRenderPass.depthImage->format(),
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                }
            }
        });


        shadowRenderPass.framebuffer = new lv::Framebuffer({
            .renderPass = shadowRenderPass.renderPass,
            .depthAttachment = {0, shadowRenderPass.depthImage}
        });
        
        shadowRenderPass.commandBuffer = new lv::CommandBuffer({});

        //Main
        mainRenderPass.colorImage = new lv::Image({
            .format = lv::Format::RGBA16Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });
        //mainRenderPass.colorSampler.filter = LV_FILTER_LINEAR;
        //mainRenderPass.colorSampler.maxLod = SSR_MIP_COUNT;
        //mainRenderPass.colorSampler.init();

        mainRenderPass.albedoMetallicImage = new lv::Image({
            .format = lv::Format::RGBA8Unorm,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
            .memoryType = LV_MEMORY_TYPE_MEMORYLESS
        });

        mainRenderPass.normalRoughnessImage = new lv::Image({
            .format = lv::Format::RGBA16Snorm,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
        });

        mainRenderPass.motionImage = new lv::Image({
            .format = lv::Format::RG16Snorm,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        mainRenderPass.f0Image = new lv::Image({
#ifdef LV_BACKEND_VULKAN //TODO: query whether the device supports this format instead
            .format = lv::Format::B10G11R11Ufloat,
#elif defined(LV_BACKEND_METAL)
            .format = lv::Format::B5G6R5Unorm,
#endif
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        mainRenderPass.depthImage = new lv::Image({
            .format = lv::Format::D32Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | LV_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | LV_IMAGE_USAGE_TRANSFER_SRC_BIT,
            .aspectMask = LV_IMAGE_ASPECT_DEPTH_BIT
        });

        mainRenderPass.halfDepthImage = new lv::Image({
            .format = lv::Format::D32Float,
            .width = uint16_t(framebufferWidth / 2),
            .height = uint16_t(framebufferHeight / 2),
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | LV_IMAGE_USAGE_TRANSFER_DST_BIT,
            .aspectMask = LV_IMAGE_ASPECT_DEPTH_BIT
        });

#ifdef LV_BACKEND_METAL
        mainRenderPass.depthAsColorImage = new lv::Image({
            .format = lv::Format::R32Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
            .memoryType = LV_MEMORY_TYPE_MEMORYLESS
        });
#endif

        mainRenderPass.gbufferSubpass = new lv::Subpass({
            .colorAttachments = {
                {1, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {2, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {3, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
#ifdef LV_BACKEND_METAL
                {6, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
#endif
            },
            .depthAttachment = {5, LV_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}
        });

        mainRenderPass.deferredSubpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {4, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            },
            .depthAttachment = {5, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL},
            .inputAttachments = {
                {1, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
                {2, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
#ifdef LV_BACKEND_VULKAN
                {5, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL}
#elif defined(LV_BACKEND_METAL)
                {6, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
#endif
            }
        });

        mainRenderPass.skyboxSubpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
            },
            .depthAttachment = {5, LV_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}
        });

        mainRenderPass.renderPass = new lv::RenderPass({
            .subpasses = {mainRenderPass.gbufferSubpass, mainRenderPass.deferredSubpass, mainRenderPass.skyboxSubpass},
            .attachments = {
                {
                    .format = mainRenderPass.colorImage->format(),
                    .index = 0,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.albedoMetallicImage->format(),
                    .index = 1,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.normalRoughnessImage->format(),
                    .index = 2,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.motionImage->format(),
                    .index = 3,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.f0Image->format(),
                    .index = 4,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.depthImage->format(),
                    .index = 5,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .initialLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                    .finalLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                },
        #ifdef LV_BACKEND_METAL
                {
                    .format = mainRenderPass.depthAsColorImage->format(),
                    .index = 6,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                }
        #endif
            }
        });

        //mainRenderPass.framebuffer.frameCount = 2;
        mainRenderPass.framebuffer = new lv::Framebuffer({
            .renderPass = mainRenderPass.renderPass,
            .colorAttachments = {
                {0, mainRenderPass.colorImage},
                {1, mainRenderPass.albedoMetallicImage},
                {2, mainRenderPass.normalRoughnessImage},
                {3, mainRenderPass.motionImage},
                {4, mainRenderPass.f0Image},
#ifdef LV_BACKEND_METAL
                {6, mainRenderPass.depthAsColorImage},
#endif
            },
            .depthAttachment = {5, mainRenderPass.depthImage}
        });

        mainRenderPass.commandBuffer = new lv::CommandBuffer({});

        //SSR
        ssrRenderPass.colorImage = new lv::Image({
            .format = lv::Format::RGBA16Float,
            .width = uint16_t(framebufferWidth / 2),
            .height = uint16_t(framebufferHeight / 2),
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        ssrRenderPass.subpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            },
            .depthAttachment = {1, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL}
        });

        ssrRenderPass.renderPass = new lv::RenderPass({
            .subpasses = {ssrRenderPass.subpass},
            .attachments = {
                {
                    .format = ssrRenderPass.colorImage->format(),
                    .index = 0,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.depthImage->format(),
                    .index = 1,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_LOAD,
                    .storeOp = LV_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                }
            }
        });

        ssrRenderPass.framebuffer = new lv::Framebuffer({
            .renderPass = ssrRenderPass.renderPass,
            .colorAttachments = {
                {0, ssrRenderPass.colorImage}
            },
            .depthAttachment = {1, mainRenderPass.halfDepthImage}
        });

        ssrRenderPass.commandBuffer = new lv::CommandBuffer({});

        //Light shaft
        lightShaftRenderPass.colorImage = new lv::Image({
            .format = lv::Format::R8Unorm,
            .width = uint16_t(framebufferWidth / 2),
            .height = uint16_t(framebufferHeight / 2),
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        lightShaftRenderPass.subpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            },
            .depthAttachment = {1, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL}
        });

        lightShaftRenderPass.renderPass = new lv::RenderPass({
            .subpasses = {lightShaftRenderPass.subpass},
            .attachments = {
                {
                    .format = lightShaftRenderPass.colorImage->format(),
                    .index = 0,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.halfDepthImage->format(),
                    .index = 1,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_LOAD,
                    .initialLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    .finalLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                }
            }
        });

        lightShaftRenderPass.framebuffer = new lv::Framebuffer({
            .renderPass = lightShaftRenderPass.renderPass,
            .colorAttachments = {
                {0, lightShaftRenderPass.colorImage}
            },
            .depthAttachment = {1, mainRenderPass.halfDepthImage}
        });

        lightShaftRenderPass.commandBuffer = new lv::CommandBuffer({});

        //Blur
        blurRenderPass.lightShaftBlurImage = new lv::Image({
            .format = lv::Format::R8Unorm,
            .width = uint16_t(framebufferWidth / 2),
            .height = uint16_t(framebufferHeight / 2),
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        blurRenderPass.subpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            }
        });

        blurRenderPass.renderPass = new lv::RenderPass({
            .subpasses = {blurRenderPass.subpass},
            .attachments = {
                {
                    .format = blurRenderPass.lightShaftBlurImage->format(),
                    .index = 0,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                }
            }
        });

        blurRenderPass.lightShaftBlurFramebuffer = new lv::Framebuffer({
            .renderPass = blurRenderPass.renderPass,
            .colorAttachments = {
                {0, blurRenderPass.lightShaftBlurImage}
            }
        });

        blurRenderPass.commandBuffer = new lv::CommandBuffer({});

        //Composite
        compositeRenderPass.ssaoImage = new lv::Image({
            .format = lv::Format::R8Unorm,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | LV_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
            .memoryType = LV_MEMORY_TYPE_MEMORYLESS
        });

        compositeRenderPass.resolvedSsrImage = new lv::Image({
            .frameCount = 2, //Set the frame count to 2 so as to be able to accumulate the results over the time
            .format = lv::Format::B10GR11Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        compositeRenderPass.resolvedSsaoImage = new lv::Image({
            .frameCount = 2,
            .format = lv::Format::R8Unorm,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        compositeRenderPass.resolvedColorImage = new lv::Image({
            .frameCount = 2,
            .format = lv::Format::RGBA16Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        compositeRenderPass.finalColorImage = new lv::Image({
            .frameCount = 2,
            .format = lv::Format::RGBA16Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        compositeRenderPass.ssaoSubpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            },
            .depthAttachment = {5, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL}
        });

        compositeRenderPass.temporalResolveSubpass = new lv::Subpass({
            .colorAttachments = {
                {1, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {2, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {3, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {4, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            },
            .depthAttachment = {5, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL},
            .inputAttachments = {
                {0, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
            }
        });
        
        compositeRenderPass.renderPass = new lv::RenderPass({
            .subpasses = {compositeRenderPass.ssaoSubpass, compositeRenderPass.temporalResolveSubpass},
            .attachments = {
                {
                    .format = compositeRenderPass.ssaoImage->format(),
                    .index = 0,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = compositeRenderPass.resolvedSsrImage->format(),
                    .index = 1,
                    //.loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = compositeRenderPass.resolvedSsaoImage->format(),
                    .index = 2,
                    //.loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = compositeRenderPass.resolvedColorImage->format(),
                    .index = 3,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = compositeRenderPass.finalColorImage->format(),
                    .index = 4,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                },
                {
                    .format = mainRenderPass.depthImage->format(),
                    .index = 5,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_LOAD,
                    .initialLayout = LV_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    .finalLayout = LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                }
            }
        });

        compositeRenderPass.framebuffer = new lv::Framebuffer({
            .frameCount = 2,
            .renderPass = compositeRenderPass.renderPass,
            .colorAttachments = {
                {
                    .index = 0,
                    .image = compositeRenderPass.ssaoImage,
                    .clearValue = {
                        .color = {
                            .float32 = {1.0f, 1.0f, 1.0f, 1.0f}
                        }
                    }
                },
                {1, compositeRenderPass.resolvedSsrImage},
                {2, compositeRenderPass.resolvedSsaoImage},
                {3, compositeRenderPass.resolvedColorImage},
                {4, compositeRenderPass.finalColorImage}
            },
            .depthAttachment = {5, mainRenderPass.depthImage}
        });

        compositeRenderPass.commandBuffer = new lv::CommandBuffer({});

        //Motion blur
        motionBlurRenderPass.colorImage = new lv::Image({
            .format = lv::Format::RGBA16Float,
            .width = framebufferWidth,
            .height = framebufferHeight,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        motionBlurRenderPass.subpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            }
        });

        motionBlurRenderPass.renderPass = new lv::RenderPass({
            .subpasses = {motionBlurRenderPass.subpass},
            .attachments = {
                {
                    .format = motionBlurRenderPass.colorImage->format(),
                    .index = 0,
                    .loadOp = LV_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                }
            }
        });

        motionBlurRenderPass.framebuffer = new lv::Framebuffer({
            .renderPass = motionBlurRenderPass.renderPass,
            .colorAttachments = {
                {0, motionBlurRenderPass.colorImage}
            }
        });

        motionBlurRenderPass.commandBuffer = new lv::CommandBuffer({});

        //Pipeline layout

        //Shadow
        shadowPipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof(glm::mat4)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_VERTEX_BIT}
                }}
            }
        });

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
                    {
                        .binding = 0,
                        .descriptorType = LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .shaderStage = LV_SHADER_STAGE_VERTEX_BIT
                    }
                }},
                {{
                    {
                        .binding = 0,
                        .descriptorType = LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    },
                    {
                        .binding = 1,
                        .descriptorType = LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    },
                    {
                        .binding = 2,
                        .descriptorType = LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    },
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
                    {
                        .binding = 0,
                        .descriptorType = LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    },
                    {
                        .binding = 1,
                        .descriptorType = LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    },
                    {
                        .binding = 2,
                        .descriptorType = LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    },
                    {
                        .binding = 3,
                        .descriptorType = LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    },
                    {
                        .binding = 4,
                        .descriptorType = LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    }
                }}
            }
        });

        //Skybox
        skyboxPipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof(PCSkyboxVP)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {
                        .binding = 0,
                        .descriptorType = LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT
                    }
                }}
            }
        });

        //SSR
        ssrPipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_FRAGMENT_BIT,
                    .offset = 0,
                    .size = sizeof(PCSsrVP)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {1, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //Light shaft
        lightShaftPipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_FRAGMENT_BIT,
                    .offset = 0,
                    .size = sizeof(PCLightShaftVP)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {1, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //Blur
        blurPipelineLayout = new lv::PipelineLayout({
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //SSAO
        ssaoPipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_FRAGMENT_BIT,
                    .offset = 0,
                    .size = sizeof(PCSsaoVP)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {1, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {2, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //Temporal resolve
        temporalResolvePipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_FRAGMENT_BIT,
                    .offset = 0,
                    .size = sizeof(PCTemporalVP)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {1, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {2, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {3, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {4, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT}
                }},
                {{
                    {0, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {1, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {2, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {3, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT}
                }},
                {{
                    {0, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {1, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {2, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //Motion blur
        motionBlurPipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_FRAGMENT_BIT,
                    .offset = 0,
                    .size = sizeof(float)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LV_SHADER_STAGE_FRAGMENT_BIT},
                    {1, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_SHADER_STAGE_FRAGMENT_BIT}
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

        //Shadow
        vertShadowShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_VERTEX_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/vertex/shadow.json").c_str())
        });

        fragShadowShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/shadow.json").c_str())
        });;

        shadowGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertShadowShaderModule,
            .fragmentShaderModule = fragShadowShaderModule,
            .pipelineLayout = shadowPipelineLayout,
            .renderPass = shadowRenderPass.renderPass,
            .vertexDescriptor = shadowVertexDescriptor,
            .depthTestEnable = lv::True,
            .cullMode = lv::CullMode::Back
        });

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
            .depthTestEnable = lv::True,
            .cullMode = lv::CullMode::Back,
            .colorBlendAttachments = {
#ifdef LV_BACKEND_METAL
                {0},
#endif
                {1},
                {2},
                {3},
#ifdef LV_BACKEND_METAL
                {4},
#endif
#ifdef LV_BACKEND_METAL
                {6}
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
            .depthTestEnable = lv::True,
            .depthWriteEnable = lv::False,
            .depthOp = LV_COMPARE_OP_NOT_EQUAL,
            .colorBlendAttachments = {
                {0},
#ifdef LV_BACKEND_METAL
                {1},
                {2},
                {3},
#endif
                {4},
#ifdef LV_BACKEND_METAL
                {6}
#endif
            }
        });

        //Skybox
        vertSkyboxShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_VERTEX_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/vertex/skybox.json").c_str())
        });

        fragSkyboxShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/skybox.json").c_str())
        });

        skyboxGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertSkyboxShaderModule,
            .fragmentShaderModule = fragSkyboxShaderModule,
            .pipelineLayout = skyboxPipelineLayout,
            .renderPass = mainRenderPass.renderPass,
            .subpassIndex = 2,
            .vertexDescriptor = skyboxVertexDescriptor,
            .depthTestEnable = lv::True,
            .depthWriteEnable = lv::False,
            .cullMode = lv::CullMode::Back,
            .colorBlendAttachments = {
                {0}
            }
        });

        //SSR
        fragSsrShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/ssr.json").c_str())
        });

        ssrGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragSsrShaderModule,
            .pipelineLayout = ssrPipelineLayout,
            .renderPass = ssrRenderPass.renderPass,
            .depthTestEnable = lv::True,
            .depthWriteEnable = lv::False,
            .depthOp = LV_COMPARE_OP_NOT_EQUAL,
            .colorBlendAttachments = {
                {0}
            }
        });

        //Light shaft
        fragLightShaftShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/light_shaft.json").c_str())
        });

        lightShaftGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragLightShaftShaderModule,
            .pipelineLayout = lightShaftPipelineLayout,
            .renderPass = lightShaftRenderPass.renderPass,
            .depthTestEnable = lv::True,
            .depthWriteEnable = lv::False,
            .depthOp = LV_COMPARE_OP_NOT_EQUAL,
            .colorBlendAttachments = {
                {0}
            }
        });

        //Blur
        fragBlurShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/blur.json").c_str())
        });

        blurGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragBlurShaderModule,
            .pipelineLayout = blurPipelineLayout,
            .renderPass = blurRenderPass.renderPass,
            .colorBlendAttachments = {
                {0}
            }
        });

        //SSAO
        fragSsaoShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/ssao.json").c_str())
        });

        ssaoGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragSsaoShaderModule,
            .pipelineLayout = ssaoPipelineLayout,
            .renderPass = compositeRenderPass.renderPass,
            .depthTestEnable = lv::True,
            .depthWriteEnable = lv::False,
            .depthOp = LV_COMPARE_OP_NOT_EQUAL,
            .colorBlendAttachments = {
                {0}
            }
        });

        //Temporal resolve
        fragTemporalResolveShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/temporal_resolve.json").c_str())
        });

        temporalResolveGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragTemporalResolveShaderModule,
            .pipelineLayout = temporalResolvePipelineLayout,
            .renderPass = compositeRenderPass.renderPass,
            .subpassIndex = 1,
            .colorBlendAttachments = {
#ifdef LV_BACKEND_METAL
                {0},
#endif
                {1},
                {2},
                {3},
                {4}
            }
        });

        //Motion blur
        fragMotionBlurShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/motion_blur.json").c_str())
        });

        motionBlurGraphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertTriangleShaderModule,
            .fragmentShaderModule = fragMotionBlurShaderModule,
            .pipelineLayout = motionBlurPipelineLayout,
            .renderPass = motionBlurRenderPass.renderPass,
            .colorBlendAttachments = {
                {0}
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

        //Uniform buffers
        shadowUniformBuffer = new lv::Buffer({
            .usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            .memoryType = LV_MEMORY_TYPE_SHARED,
            .size = sizeof(glm::mat4) * SHADOW_CASCADE_COUNT
        });

        gbufferUniformBuffer = new lv::Buffer({
            .usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            .memoryType = LV_MEMORY_TYPE_SHARED,
            .size = sizeof(UBOGBufferVP)
        });

        lightUniformBuffer = new lv::Buffer({
            .frameCount = 1,
            .usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT | LV_BUFFER_USAGE_TRANSFER_DST_BIT,
            .size = sizeof(UBOLight)
        });

        //Vertex and index buffers
        glm::vec3 skyboxVertices[] = {
            {-1.0f,  1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
            { 1.0f, -1.0f, -1.0f},
            { 1.0f,  1.0f, -1.0f},

            {-1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f,  1.0f},

            { 1.0f, -1.0f, -1.0f},
            { 1.0f, -1.0f,  1.0f},
            { 1.0f,  1.0f,  1.0f},
            { 1.0f,  1.0f, -1.0f},

            {-1.0f, -1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},
            { 1.0f,  1.0f,  1.0f},
            { 1.0f, -1.0f,  1.0f},

            {-1.0f,  1.0f, -1.0f},
            { 1.0f,  1.0f, -1.0f},
            { 1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},

            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
            { 1.0f, -1.0f,  1.0f},
            { 1.0f, -1.0f, -1.0f}
        };

        uint16_t skyboxIndices[] = {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            8, 9, 10,
            8, 10, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23
        };

        //Light
        light.color = glm::vec4(/*250 / 255.0f, 203 / 255.0f, 92 / 255.0f*/1.0f, 1.0f, 1.0f, 3.0f);
        light.direction = glm::vec4(glm::normalize(glm::vec3(3.0, -4.0, 1.0)), 1.0f);

        precomputeSkylight();

        //Copy commands
        lv::CommandBuffer* copyCommandBuffer = new lv::CommandBuffer({
            .flags = LV_COMMAND_BUFFER_CREATE_FENCE_TO_WAIT_UNTIL_COMPLETE_BIT
        });
        copyCommandBuffer->beginRecording();
        copyCommandBuffer->beginBlitCommands();

        //Transition attachments to the correct layout
        copyCommandBuffer->cmdTransitionImageLayout(mainRenderPass.depthImage, 0, LV_IMAGE_LAYOUT_UNDEFINED, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

        //Transition images that are used in composite render pass for temporal accumulation
        for (uint8_t i = 1; i < swapChain->maxFramesInFlight(); i++) { //Skip the first one, since it will be used as an attachment in the first frame
            copyCommandBuffer->cmdTransitionImageLayout(ssrRenderPass.colorImage, i, LV_IMAGE_LAYOUT_UNDEFINED, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            copyCommandBuffer->cmdTransitionImageLayout(compositeRenderPass.ssaoImage, i, LV_IMAGE_LAYOUT_UNDEFINED, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        //Copy to data
        copyCommandBuffer->cmdStagingCopyDataToBuffer(lightUniformBuffer, &light);

        skyboxVertexBuffer = new lv::Buffer({
            .frameCount = 1,
            .usage = LV_BUFFER_USAGE_VERTEX_BUFFER_BIT | LV_BUFFER_USAGE_TRANSFER_DST_BIT,
            .size = sizeof(skyboxVertices)
        });
        copyCommandBuffer->cmdStagingCopyDataToBuffer(skyboxVertexBuffer, skyboxVertices);

        skyboxIndexBuffer = new lv::Buffer({
            .frameCount = 1,
            .usage = LV_BUFFER_USAGE_INDEX_BUFFER_BIT | LV_BUFFER_USAGE_TRANSFER_DST_BIT,
            .size = sizeof(skyboxIndices)
        });
        copyCommandBuffer->cmdStagingCopyDataToBuffer(skyboxIndexBuffer, skyboxIndices);

        //Textures
        std::default_random_engine rndEngine((unsigned)time(0));
        std::uniform_real_distribution<int8_t> rndInt8Dist(-128, 127);

        std::vector<glm::i8vec2> ssaoNoise(AO_NOISE_TEX_SIZE * AO_NOISE_TEX_SIZE);
        for (uint32_t i = 0; i < ssaoNoise.size(); i++) {
            ssaoNoise[i] = glm::i8vec2(rndInt8Dist(rndEngine), rndInt8Dist(rndEngine));
        }

        std::uniform_real_distribution<uint8_t> rndUint8Dist(0, 255);

        std::vector<uint8_t> hbaoNoise(AO_NOISE_TEX_SIZE * AO_NOISE_TEX_SIZE);
        for (uint32_t i = 0; i < hbaoNoise.size(); i++) {
            hbaoNoise[i] = rand() % 255;//rndUint8Dist(rndEngine);
            /*
            while (glm::length2(rndVec) > 1.0f) {
                rndVec = glm::normalize(glm::vec2(rndFloatDist(rndEngine), rndFloatDist(rndEngine)));
            }
            */
            //uint8_t index = rand() % 2;
            //rndVec[(index + 1) % 2] /= fabs(rndVec[index]);
            //rndVec[index] = rndVec[index] > 0.0f ? 1.0f : -1.0f;
            //hbaoNoise[i] = glm::i8vec2(rndVec[0] * 127, rndVec[1] * 127);
            //std::cout << "Noise " << i << ": " << (int)hbaoNoise[i] << std::endl;
        }

        aoNoiseImage = new lv::Image({
            .frameCount = 1,
            .format = (AO_TYPE == AO_TYPE_SSAO ? lv::Format::RG8Snorm : lv::Format::R8Unorm),
            .width = AO_NOISE_TEX_SIZE,
            .height = AO_NOISE_TEX_SIZE,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_TRANSFER_DST_BIT
        });
        if (AO_TYPE == AO_TYPE_SSAO)
            copyCommandBuffer->cmdStagingCopyDataToImage(aoNoiseImage, ssaoNoise.data(), 2);
        else
            copyCommandBuffer->cmdStagingCopyDataToImage(aoNoiseImage, hbaoNoise.data(), 1);
        copyCommandBuffer->cmdTransitionImageLayout(aoNoiseImage, 0, LV_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        brdfLUTImage = new lv::Image({"../examples/assets/textures/brdf_lut.png"}, copyCommandBuffer);
        copyCommandBuffer->cmdTransitionImageLayout(brdfLUTImage, 0, LV_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        //Descriptor sets
        shadowDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = shadowPipelineLayout,
            .bufferBindings = {
                shadowUniformBuffer->descriptorInfo(0)
            }
        });

        gbufferDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = gbufferPipelineLayout,
            .bufferBindings = {
                gbufferUniformBuffer->descriptorInfo(0)
            }
        });

        deferredDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = deferredPipelineLayout,
            .bufferBindings = {
                lightUniformBuffer->descriptorInfo(4)
            },
            .imageBindings = {
                mainRenderPass.albedoMetallicImage->descriptorInfo(0, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT),
                mainRenderPass.normalRoughnessImage->descriptorInfo(1, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT),
#ifdef LV_BACKEND_VULKAN
                mainRenderPass.depthImage->descriptorInfo(2, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL),
#elif defined(LV_BACKEND_METAL)
                mainRenderPass.depthAsColorImage->descriptorInfo(2, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT),
#endif
                shadowRenderPass.depthSampler->descriptorInfo(shadowRenderPass.depthImage, 3)
            }
        });

        skyboxDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = skyboxPipelineLayout,
            .imageBindings = {
                basicSampler->descriptorInfo(skylightImage, 0)
            }
        });

        ssrDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = ssrPipelineLayout,
            .imageBindings = {
                basicSampler->descriptorInfo(mainRenderPass.halfDepthImage, 0, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL),
                mainRenderPass.normalRoughnessImage->descriptorInfo(1)
            }
        });

        lightShaftDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = lightShaftPipelineLayout,
            .imageBindings = {
                mainRenderPass.halfDepthImage->descriptorInfo(0, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL),
                shadowRenderPass.depthSampler->descriptorInfo(shadowRenderPass.depthImage, 1)
            }
        });

        lightShaftBlurDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = blurPipelineLayout,
            .imageBindings = {
                lightShaftRenderPass.colorImage->descriptorInfo(0)
            }
        });

        ssaoDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = ssaoPipelineLayout,
            .imageBindings = {
                basicSampler->descriptorInfo(mainRenderPass.depthImage, 0, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL),
                mainRenderPass.normalRoughnessImage->descriptorInfo(1),
                repeatSampler->descriptorInfo(aoNoiseImage, 2)
            }
        });

        temporalResolveDescriptorSet1 = new lv::DescriptorSet({
            .pipelineLayout = temporalResolvePipelineLayout,
            .imageBindings = {
                basicSampler->descriptorInfo(mainRenderPass.colorImage, 0),
                basicSampler->descriptorInfo(compositeRenderPass.resolvedSsrImage, 1, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, -1),
                basicSampler->descriptorInfo(compositeRenderPass.resolvedSsaoImage, 2, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, -1),
                basicSampler->descriptorInfo(compositeRenderPass.resolvedColorImage, 3, LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, -1),
                basicSampler->descriptorInfo(brdfLUTImage, 4)
            }
        });

        temporalResolveDescriptorSet2 = new lv::DescriptorSet({
            .pipelineLayout = temporalResolvePipelineLayout,
            .layoutIndex = 1,
            .imageBindings = {
                mainRenderPass.depthImage->descriptorInfo(0, LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL),
                mainRenderPass.normalRoughnessImage->descriptorInfo(1),
                mainRenderPass.motionImage->descriptorInfo(2),
                mainRenderPass.f0Image->descriptorInfo(3)
            }
        });

        temporalResolveDescriptorSet3 = new lv::DescriptorSet({
            .pipelineLayout = temporalResolvePipelineLayout,
            .layoutIndex = 2,
            .imageBindings = {
                ssrRenderPass.colorImage->descriptorInfo(0),
                blurRenderPass.lightShaftBlurImage->descriptorInfo(1),
                compositeRenderPass.ssaoImage->descriptorInfo(2, LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
            }
        });

        motionBlurDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = motionBlurPipelineLayout,
            .imageBindings = {
                basicSampler->descriptorInfo(compositeRenderPass.finalColorImage, 0),
                mainRenderPass.motionImage->descriptorInfo(1)
            }
        });

        hdrDescriptorSet = new lv::DescriptorSet({
            .pipelineLayout = hdrPipelineLayout,
            .imageBindings = {
                motionBlurRenderPass.colorImage->descriptorInfo(0)
            }
        });

        //Camera
        uint16_t width, height;
        lvndGetWindowSize(window, &width, &height);
        camera.width = framebufferWidth;
        camera.height = framebufferHeight;
        camera.aspectRatio = (float)width / (float)height;

        //Model
        sponzaModel.scale = glm::vec3(0.01f);
        sponzaModel.init(copyCommandBuffer, gbufferPipelineLayout, nullptr, "../examples/assets/models/sponza/scene.gltf", 3, 1, 0); //TODO: make the model class play nicely with the rest of the API
        //backpackModel.scale = glm::vec3(32.0f, 2.0f, 32.0f);
        //backpackModel.init(gbufferPipelineLayout, "../assets/models/pan/brass_pan_01_4k.gltf", 3, 1);

        //Commit copy commands
        copyCommandBuffer->endRecording();
        copyCommandBuffer->submit();
        copyCommandBuffer->waitUntilCompleted();
        delete copyCommandBuffer;

        //Halton sequence
        /*
        std::cout << "#define HALTON_COUNT 128" << std::endl;
        std::cout << "const vec2 haltonSequence[HALTON_COUNT] = {" << std::endl;
        for (int i = 0; i < 128; i++) {
            glm::vec2 halton = glm::vec2(createHaltonSequence(i + 1, 2), createHaltonSequence(i + 1, 3));
            std::cout << "  {" << halton.x << ", " << halton.y << "}," << std::endl;
        }
        std::cout << "};" << std::endl;

        //SSAO Kernel
        std::random_device randomDevice;
        std::cout << "const vec3 SSAO_KERNEL[SSAO_SAMPLE_SET_COUNT][SSAO_KERNEL_SIZE] = {" << std::endl;
        for (uint8_t i = 0; i < SSAO_SAMPLE_SET_COUNT; i++) {
            std::seed_seq seed{randomDevice()};
            std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
            std::default_random_engine generator(seed);
            std::vector<glm::vec3> ssaoKernel(SSAO_KERNEL_SIZE);
            for (uint8_t j = 0; j < SSAO_KERNEL_SIZE; ++j) {
                glm::vec3 sample(
                    randomFloats(generator) * 2.0 - 1.0, 
                    randomFloats(generator) * 2.0 - 1.0, 
                    randomFloats(generator)
                );
                sample  = glm::normalize(sample);
                sample *= randomFloats(generator);
                
                float scale = (float)j / (float)SSAO_KERNEL_SIZE; 
                scale = lerp(0.1f, 1.0f, scale * scale);
                ssaoKernel[j] = sample * scale;
            }
            
            std::cout << "  {" << std::endl;
            for (uint8_t j = 0; j < ssaoKernel.size(); j++) {
                std::cout << "      {" << ssaoKernel[j].x << ", " << ssaoKernel[j].y << ", " << ssaoKernel[j].z << "}," << std::endl;
            }
            std::cout << "  }," << std::endl;
        }
        std::cout << "};" << std::endl;
        */
    }

    ~LavaCoreExampleApp() {
        sponzaModel.destroy();
        //backpackModel.destroy();

        delete shadowDescriptorSet;
        delete gbufferDescriptorSet;
        delete deferredDescriptorSet;
        delete skyboxDescriptorSet;
        delete ssrDescriptorSet;
        delete ssaoDescriptorSet;
        delete lightShaftDescriptorSet;
        delete lightShaftBlurDescriptorSet;
        delete temporalResolveDescriptorSet1;
        delete temporalResolveDescriptorSet2;
        delete temporalResolveDescriptorSet3;
        delete motionBlurDescriptorSet;
        delete hdrDescriptorSet;

        delete basicSampler;
        delete repeatSampler;

        delete aoNoiseImage;
        delete brdfLUTImage;
        delete skylightImage;

        delete shadowUniformBuffer;
        delete gbufferUniformBuffer;

        delete skyboxVertexBuffer;
        delete skyboxIndexBuffer;

        delete shadowGraphicsPipeline;
        delete gbufferGraphicsPipeline;
        delete deferredGraphicsPipeline;
        delete skyboxGraphicsPipeline;
        delete ssrGraphicsPipeline;
        delete ssaoGraphicsPipeline;
        delete lightShaftGraphicsPipeline;
        delete blurGraphicsPipeline;
        delete temporalResolveGraphicsPipeline;
        delete motionBlurGraphicsPipeline;
        delete hdrGraphicsPipeline;

        delete shadowPipelineLayout;
        delete gbufferPipelineLayout;
        delete deferredPipelineLayout;
        delete skyboxPipelineLayout;
        delete ssrPipelineLayout;
        delete ssaoPipelineLayout;
        delete lightShaftPipelineLayout;
        delete blurPipelineLayout;
        delete temporalResolvePipelineLayout;
        delete motionBlurPipelineLayout;
        delete hdrPipelineLayout;

        //TODO: destroy render passes

        delete swapChain;
        delete device;
        delete instance;
    }

    void update() override {
        float crntTime = lvndGetTime();
        float dt = crntTime - prevTime;
        prevTime = crntTime;

        if (lvndGetKeyState(window, LVND_KEY_SPACE) == LVND_STATE_PRESSED)
            frameIndex = 0;

        swapChain->acquireNextImage();
        
        camera.inputs(window, dt);
        camera.applyMovement();
        //camera.position.x = sin(lvndGetTime() * 2.0f) * 2.0f;
        //camera.position.y = cos(lvndGetTime() * 2.0f) * 2.0f;
        camera.loadViewProj();

        //Rendering

        getLightMatrices(light.direction);

        sponzaModel.calculateModel();
        //backpackModel.position.y = 2.0f + sin(lvndGetTime() * 12.0f) * 2.0f;
        //backpackModel.calculateModel();

        //Shadow pass
        std::cout << "SHADOW PASS" << std::endl;
        shadowRenderPass.commandBuffer->beginRecording();
        shadowRenderPass.commandBuffer->beginRenderCommands(shadowRenderPass.framebuffer);

        shadowRenderPass.commandBuffer->cmdBindGraphicsPipeline(shadowGraphicsPipeline);

        shadowUniformBuffer->copyDataTo(&deferredVP.shadowViewProjs);

        shadowRenderPass.commandBuffer->cmdBindDescriptorSet(shadowDescriptorSet);

        sponzaModel.renderNoTextures(shadowRenderPass.commandBuffer, SHADOW_CASCADE_COUNT);
        
        shadowRenderPass.commandBuffer->endRecording();
        shadowRenderPass.commandBuffer->submit();

        //Main pass
        std::cout << "MAIN PASS" << std::endl;

        //GBuffer subpass
        mainRenderPass.commandBuffer->beginRecording();
        mainRenderPass.commandBuffer->beginRenderCommands(mainRenderPass.framebuffer);

        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(gbufferGraphicsPipeline);

        UBOGBufferVP gbufferVP;
        gbufferVP.viewProj = camera.viewProj;
        gbufferVP.prevViewProj = camera.prevViewProj;
        gbufferVP.frameIndex = frameIndex;

        gbufferUniformBuffer->copyDataTo(&gbufferVP);

        mainRenderPass.commandBuffer->cmdBindDescriptorSet(gbufferDescriptorSet);
        
        sponzaModel.render(mainRenderPass.commandBuffer);
        //backpackModel.render(gbufferGraphicsPipeline);

        //Deferred subpass
        mainRenderPass.commandBuffer->cmdNextSubpass();

        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(deferredGraphicsPipeline);

        deferredVP.invViewProj = glm::inverse(camera.viewProj);
        deferredVP.viewPos = camera.position;

        mainRenderPass.commandBuffer->cmdPushConstants(&deferredVP, 0);

        mainRenderPass.commandBuffer->cmdBindDescriptorSet(deferredDescriptorSet);

        mainRenderPass.commandBuffer->cmdDraw(3);

        //Skybox
        mainRenderPass.commandBuffer->cmdNextSubpass();

        mainRenderPass.commandBuffer->cmdBindGraphicsPipeline(skyboxGraphicsPipeline);

        PCSkyboxVP skyboxVP;
        skyboxVP.viewProj = camera.projection * glm::mat4(glm::mat3(camera.view));
        skyboxVP.prevViewProj = camera.projection * glm::mat4(glm::mat3(camera.prevView));

        mainRenderPass.commandBuffer->cmdPushConstants(&skyboxVP, 0);

        mainRenderPass.commandBuffer->cmdBindDescriptorSet(skyboxDescriptorSet);

        mainRenderPass.commandBuffer->cmdBindVertexBuffer(skyboxVertexBuffer);
        mainRenderPass.commandBuffer->cmdDrawIndexed(skyboxIndexBuffer, LV_INDEX_TYPE_UINT16, skyboxIndexBuffer->size() / sizeof(uint16_t));

        mainRenderPass.commandBuffer->beginComputeCommands();

        //mainRenderPass.colorImage.generateMipmaps(1);
        //mainRenderPass.depthImage.generateMipmaps(1);
        mainRenderPass.commandBuffer->cmdTransitionImageLayout(mainRenderPass.depthImage, swapChain->crntFrame(), LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, LV_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        mainRenderPass.commandBuffer->cmdBlitToImageFromImage(mainRenderPass.depthImage, mainRenderPass.halfDepthImage); //TODO: probably rename this function, it may seem a bit weird that we begin 'compute' commands instead of 'blit' commands when blitting to an image
        mainRenderPass.commandBuffer->cmdTransitionImageLayout(mainRenderPass.halfDepthImage, swapChain->crntFrame(), LV_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, LV_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

        mainRenderPass.commandBuffer->endRecording();
        mainRenderPass.commandBuffer->submit();

        //SSR pass
        std::cout << "SSR PASS" << std::endl;
        ssrRenderPass.commandBuffer->beginRecording();
        ssrRenderPass.commandBuffer->beginRenderCommands(ssrRenderPass.framebuffer);

        ssrRenderPass.commandBuffer->cmdBindGraphicsPipeline(ssrGraphicsPipeline);

        PCSsrVP ssrVP;
        ssrVP.viewProj = camera.viewProj;
        ssrVP.invViewProj = camera.invViewProj;
        ssrVP.view = camera.view;
        ssrVP.viewPos = camera.position;
        ssrVP.frameIndex = frameIndex;

        ssrRenderPass.commandBuffer->cmdPushConstants(&ssrVP, 0);

        ssrRenderPass.commandBuffer->cmdBindDescriptorSet(ssrDescriptorSet);

        ssrRenderPass.commandBuffer->cmdDraw(3);

        ssrRenderPass.commandBuffer->endRecording();
        ssrRenderPass.commandBuffer->submit();

        //Light shaft pass
        std::cout << "LIGHT SHAFT PASS" << std::endl;
        lightShaftRenderPass.commandBuffer->beginRecording();
        lightShaftRenderPass.commandBuffer->beginRenderCommands(lightShaftRenderPass.framebuffer);

        lightShaftRenderPass.commandBuffer->cmdBindGraphicsPipeline(lightShaftGraphicsPipeline);

        lightShaftRenderPass.commandBuffer->cmdBindDescriptorSet(lightShaftDescriptorSet);

        PCLightShaftVP lightShaftVP;
        lightShaftVP.viewProj = camera.viewProj;
        lightShaftVP.invViewProj = camera.invViewProj;
        for (uint8_t i = 0; i < SHADOW_CASCADE_COUNT; i++)
            lightShaftVP.shadowViewProjs[i] = deferredVP.shadowViewProjs[i];
        lightShaftVP.viewPos = camera.position;

        lightShaftRenderPass.commandBuffer->cmdPushConstants(&lightShaftVP, 0);

        lightShaftRenderPass.commandBuffer->cmdDraw(3);

        lightShaftRenderPass.commandBuffer->endRecording();
        lightShaftRenderPass.commandBuffer->submit();

        //Blur pass
        std::cout << "BLUR PASS" << std::endl;
        blurRenderPass.commandBuffer->beginRecording();

        //Light shaft blur
        blurRenderPass.commandBuffer->beginRenderCommands(blurRenderPass.lightShaftBlurFramebuffer);

        blurRenderPass.commandBuffer->cmdBindGraphicsPipeline(blurGraphicsPipeline);

        blurRenderPass.commandBuffer->cmdBindDescriptorSet(lightShaftBlurDescriptorSet);

        blurRenderPass.commandBuffer->cmdDraw(3);

        blurRenderPass.commandBuffer->endRecording();
        blurRenderPass.commandBuffer->submit();

        //Composite pass
        std::cout << "COMPOSITE PASS" << std::endl;
        compositeRenderPass.commandBuffer->beginRecording();
        compositeRenderPass.commandBuffer->beginRenderCommands(compositeRenderPass.framebuffer);

        //SSAO subpass
        compositeRenderPass.commandBuffer->cmdBindGraphicsPipeline(ssaoGraphicsPipeline);

        compositeRenderPass.commandBuffer->cmdBindDescriptorSet(ssaoDescriptorSet);

        PCSsaoVP ssaoVP;
        ssaoVP.projection = camera.projection;
        ssaoVP.view = camera.view;
        ssaoVP.invViewProj = camera.invViewProj;
        ssaoVP.frameIndex = frameIndex;

        compositeRenderPass.commandBuffer->cmdPushConstants(&ssaoVP, 0);

        compositeRenderPass.commandBuffer->cmdDraw(3);

        //Temporal resolve subpass
        compositeRenderPass.commandBuffer->cmdNextSubpass();

        compositeRenderPass.commandBuffer->cmdBindGraphicsPipeline(temporalResolveGraphicsPipeline);

        compositeRenderPass.commandBuffer->cmdBindDescriptorSet(temporalResolveDescriptorSet1);
        compositeRenderPass.commandBuffer->cmdBindDescriptorSet(temporalResolveDescriptorSet2);
        compositeRenderPass.commandBuffer->cmdBindDescriptorSet(temporalResolveDescriptorSet3);

        PCTemporalVP temporalVP;
        //temporalVP.projection = camera.projection;
        //temporalVP.prevInvProj = glm::inverse(prevProj);
        temporalVP.viewProj = camera.viewProj;
        temporalVP.invViewProj = camera.invViewProj;
        //temporalVP.prevInvViewProj = camera.prevInvViewProj;
        //temporalVP.invView = glm::inverse(camera.view);
        //temporalVP.invProj = glm::inverse(camera.projection);
        //temporalVP.prevView = prevView;
        temporalVP.lightColor = light.color;
        temporalVP.viewPos = camera.position;
        float reflectionTemporalDividerNumerator = glm::distance(camera.position, camera.prevPosition);
        float reflectionTemporalDividerDenominator = reflectionTemporalDividerNumerator + (-glm::dot(camera.direction, camera.prevDirection) * 0.5f + 0.5f);
        temporalVP.reflectionTemporalDivider = reflectionTemporalDividerNumerator / reflectionTemporalDividerDenominator;
        //std::cout << reflectionTemporalDividerNumerator << " / " << reflectionTemporalDividerDenominator << " = " << temporalVP.reflectionTemporalDivider << std::endl;
        temporalVP.frameIndex = frameIndex;
        temporalVP.useSpatialFilter = (lvndGetKeyState(window, LVND_KEY_P) != LVND_STATE_PRESSED);
        //std::cout << "Spatial: " << temporalVP.useSpatialFilter << std::endl;

        compositeRenderPass.commandBuffer->cmdPushConstants(&temporalVP, 0);

        compositeRenderPass.commandBuffer->cmdDraw(3);

        compositeRenderPass.commandBuffer->endRecording();
        compositeRenderPass.commandBuffer->submit();

        //Motion blur pass
        std::cout << "MOTION BLUR PASS" << std::endl;
        motionBlurRenderPass.commandBuffer->beginRecording();
        motionBlurRenderPass.commandBuffer->beginRenderCommands(motionBlurRenderPass.framebuffer);

        motionBlurRenderPass.commandBuffer->cmdBindGraphicsPipeline(motionBlurGraphicsPipeline);

        motionBlurRenderPass.commandBuffer->cmdBindDescriptorSet(motionBlurDescriptorSet);

        motionBlurRenderPass.commandBuffer->cmdPushConstants(&dt, 0);

        motionBlurRenderPass.commandBuffer->cmdDraw(3);

        motionBlurRenderPass.commandBuffer->endRecording();
        motionBlurRenderPass.commandBuffer->submit();

        //delete skylightImage;
        //precomputeSkylight();

        //HDR pass
        std::cout << "HDR PASS" << std::endl;
        swapChain->commandBuffer()->beginRecording();
        swapChain->commandBuffer()->beginRenderCommands(swapChain->framebuffer());

        swapChain->commandBuffer()->cmdBindGraphicsPipeline(hdrGraphicsPipeline);

        swapChain->commandBuffer()->cmdBindDescriptorSet(hdrDescriptorSet);

        swapChain->commandBuffer()->cmdDraw(3);

        swapChain->commandBuffer()->endRecording();
        swapChain->renderAndPresent();

        frameIndex++;
    }

    void precomputeSkylight() {
        skylightImage = new lv::Image({
            .frameCount = 1,
            .format = lv::Format::B10GR11Float,
            .width = SKYLIGHT_IMAGE_SIZE,
            .height = SKYLIGHT_IMAGE_SIZE,
            .imageType = lv::ImageType::Cube,
            .usage = LV_IMAGE_USAGE_SAMPLED_BIT | LV_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        });

        lv::Subpass* subpass = new lv::Subpass({
            .colorAttachments = {
                {0, LV_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
            }
        });

        lv::RenderPass* renderPass = new lv::RenderPass({
            .subpasses = {subpass},
            .attachments = {
                {
                    .index = 0,
                    .format = skylightImage->format(),
                    .storeOp = LV_ATTACHMENT_STORE_OP_STORE,
                    .finalLayout = LV_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                }
            }
        });

        lv::Framebuffer* framebuffer = new lv::Framebuffer({
            .frameCount = 1,
            .renderPass = renderPass,
            .colorAttachments = {
                {0, skylightImage}
            }
        });

        lv::CommandBuffer* commandBuffer = new lv::CommandBuffer({
            .frameCount = 1
        });

        //Pipeline layout
        lv::PipelineLayout* pipelineLayout = new lv::PipelineLayout({
            .pushConstantRanges = {
                {
                    .stageFlags = LV_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof(PCSkylightVP)
                }
            },
            .descriptorSetLayouts = {
                {{
                    {0, LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LV_SHADER_STAGE_FRAGMENT_BIT}
                }}
            }
        });

        //Graphics pipeline
        lv::ShaderModule* vertShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_VERTEX_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/vertex/skylight.json").c_str())
        });

        lv::ShaderModule* fragShaderModule = new lv::ShaderModule({
            .shaderStage = LV_SHADER_STAGE_FRAGMENT_BIT,
            .source = lv::readFile((assetDir + "/shaders/compiled/fragment/skylight.json").c_str())
        });

        lv::GraphicsPipeline* graphicsPipeline = new lv::GraphicsPipeline({
            .vertexShaderModule = vertShaderModule,
            .fragmentShaderModule = fragShaderModule,
            .pipelineLayout = pipelineLayout,
            .renderPass = renderPass,
            .colorBlendAttachments = {
                {0}
            }
        });

        //delete vertShaderModule; //TODO: uncomment
        //delete fragShaderModule;

        //Uniform buffer
        glm::vec3 lightPos = -light.direction;

        lv::Buffer* uniformBuffer = new lv::Buffer({
            .frameCount = 1,
            .usage = LV_BUFFER_USAGE_UNIFORM_BUFFER_BIT | LV_BUFFER_USAGE_TRANSFER_DST_BIT,
            .size = sizeof(glm::vec3)
        });

        //Descriptor set
        lv::DescriptorSet* descriptorSet = new lv::DescriptorSet({
            .frameCount = 1,
            .pipelineLayout = pipelineLayout,
            .bufferBindings = {
                uniformBuffer->descriptorInfo(0)
            }
        });

        //Bake
        commandBuffer->beginRecording();
        commandBuffer->beginBlitCommands();

        commandBuffer->cmdStagingCopyDataToBuffer(uniformBuffer, &lightPos);

        commandBuffer->beginRenderCommands(framebuffer);

        commandBuffer->cmdBindGraphicsPipeline(graphicsPipeline);
        
        commandBuffer->cmdBindDescriptorSet(descriptorSet);

        const glm::vec3 cubeForwardDirections[6] = {
            { 1.0f,  0.0f,  0.0f},
            {-1.0f,  0.0f,  0.0f},
            { 0.0f,  1.0f,  0.0f},
            { 0.0f, -1.0f,  0.0f},
            { 0.0f,  0.0f, -1.0f},
            { 0.0f,  0.0f,  1.0f}
        };
        const glm::vec3 cubeUpDirections[6] = {
            { 0.0f,  1.0f,  0.0f},
            { 0.0f,  1.0f,  0.0f},
            { 0.0f,  0.0f,  1.0f},
            { 0.0f,  0.0f, -1.0f},
            { 0.0f,  1.0f,  0.0f},
            { 0.0f,  1.0f,  0.0f}
        };

        for (uint8_t i = 0; i < 6; i++) {
            PCSkylightVP skylightVP;
            skylightVP.view = glm::lookAt(glm::vec3(0.0f), cubeForwardDirections[i], cubeUpDirections[i]);
            skylightVP.layerIndex = i;

            commandBuffer->cmdPushConstants(&skylightVP, 0);

            commandBuffer->cmdDraw(3);
        }

        commandBuffer->endRecording();
        commandBuffer->submit();

        //Destroy
        delete commandBuffer;
        delete framebuffer;
        delete renderPass;
    }

    static std::vector<glm::vec4> getFrustumCorners(const glm::mat4& proj, const glm::mat4& view) {
        std::vector<glm::vec4> frustumCorners;
        const auto inv = glm::inverse(proj * view);

        for (unsigned int x = 0; x < 2; ++x) {
            for (unsigned int y = 0; y < 2; ++y) {
                for (unsigned int z = 0; z < 2; ++z) {
                    const glm::vec4 pt =
                    inv * glm::vec4(
                        (2.0f * x - 1.0f),
                        (2.0f * y - 1.0f),
                        (2.0f * z - 1.0f),
                        1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    void getLightMatrix(glm::vec3 lightDir, float nearPlane, float farPlane, uint8_t index) {
        glm::mat4 lightProj = glm::perspective(glm::radians(camera.fov), camera.aspectRatio, nearPlane, farPlane);

        std::vector<glm::vec4> frustumCorners = getFrustumCorners(lightProj, camera.view);

        //Get frustum center
        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
        for (auto &corner : frustumCorners) {
            center += glm::vec3(corner);
        }
        center /= frustumCorners.size();

        glm::mat4 view = glm::lookAt(center, center + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));

        //Get min/max corner coordinates
        float maxX = std::numeric_limits<float>::min();
        float maxY = std::numeric_limits<float>::min();
        float maxZ = std::numeric_limits<float>::min();
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float minZ = std::numeric_limits<float>::max();

        for (auto &corner : frustumCorners) {
            auto trf = view * corner;

            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        float zAdd = camera.farPlane * 0.8f;
        minZ -= zAdd;
        maxZ += zAdd;

        glm::mat4 projection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

        deferredVP.shadowViewProjs[index] = projection * view;
    }

    void getLightMatrices(glm::vec3 lightDir) {
        for (int i = 0; i < SHADOW_CASCADE_COUNT; i++) {
            if (i == 0) {
                getLightMatrix(lightDir, camera.nearPlane, cascadeLevels[0], 0);
            } else {
                getLightMatrix(lightDir, cascadeLevels[i - 1], cascadeLevels[i], i);
            }
        }
    }
};

int main() {
    LavaCoreExampleApp application;
    application.run();

    return 0;
}
