#include "vulkan/lvcore/render_pass.hpp"

#include "vulkan/lvcore/common.hpp"

#include "vulkan/lvcore/swap_chain.hpp"

namespace lv {

namespace vulkan {

VkAttachmentDescription getAttachmentDescription(RenderPassAttachment& attachment) {
    VkFormat vkFormat;
    GET_VK_FORMAT(attachment.format, vkFormat);
    VkAttachmentLoadOp vkAttachmentLoadOp;
    GET_VK_ATTACHMENT_LOAD_OP(attachment.loadOp, vkAttachmentLoadOp);
    VkAttachmentStoreOp vkAttachmentStoreOp;
    GET_VK_ATTACHMENT_STORE_OP(attachment.storeOp, vkAttachmentStoreOp);
    VkImageLayout vkInitialImageLayout, vkFinalImageLayout;
    GET_VK_IMAGE_LAYOUT(attachment.initialLayout, vkInitialImageLayout);
    GET_VK_IMAGE_LAYOUT(attachment.finalLayout, vkFinalImageLayout);

    VkAttachmentDescription description{};
    description.format = vkFormat;
    description.samples = VK_SAMPLE_COUNT_1_BIT;
    description.loadOp = vkAttachmentLoadOp;
    description.storeOp = vkAttachmentStoreOp;
    description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout = vkInitialImageLayout;
    description.finalLayout = vkFinalImageLayout;

    return description;
}

VkAttachmentReference getAttachmentReference(SubpassAttachment& attachment) {
    VkImageLayout vkImageLayout;
    GET_VK_IMAGE_LAYOUT(attachment.layout, vkImageLayout);

    VkAttachmentReference reference{};
    reference.attachment = attachment.index;
    reference.layout = vkImageLayout;

    return reference;
}

RenderPass::RenderPass(RenderPassCreateInfo createInfo) : dependencies(createInfo.dependencies) {
    std::vector<VkAttachmentDescription> attachmentDescriptions(createInfo.attachments.size());
    for (int i = 0; i < createInfo.attachments.size(); i++)
        attachmentDescriptions[createInfo.attachments[i].index] = getAttachmentDescription(createInfo.attachments[i]);

    /*
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    */
    
    /*
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    */

    std::vector<VkSubpassDescription> subpassDescs(createInfo.subpasses.size());
    std::vector<std::vector<VkAttachmentReference> > colorAttachmentReferences(createInfo.subpasses.size());
    std::vector<VkAttachmentReference> depthAttachmentReferences(createInfo.subpasses.size());
    std::vector<std::vector<VkAttachmentReference> > inputAttachmentReferences(createInfo.subpasses.size());
    for (uint8_t i = 0; i < createInfo.subpasses.size(); i++) {
        colorAttachmentReferences[i].resize(createInfo.subpasses[i]->colorAttachments.size());
        for (int j = 0; j < colorAttachmentReferences[i].size(); j++)
            colorAttachmentReferences[i][j] = getAttachmentReference(createInfo.subpasses[i]->colorAttachments[j]);

        if (createInfo.subpasses[i]->depthAttachment.index != -1)
            depthAttachmentReferences[i] = getAttachmentReference(createInfo.subpasses[i]->depthAttachment);

        inputAttachmentReferences[i].resize(createInfo.subpasses[i]->inputAttachments.size());
        for (int j = 0; j < inputAttachmentReferences[i].size(); j++)
            inputAttachmentReferences[i][j] = getAttachmentReference(createInfo.subpasses[i]->inputAttachments[j]);

        //std::cout << "Color attachments: " << colorAttachmentReferences[i].size() << std::endl;
        //std::cout << "Depth attachment: " << (subpasses[i]->depthAttachment.index != -1) << std::endl;
        //std::cout << "Input attachments: " << inputAttachmentReferences[i].size() << std::endl;

        subpassDescs[i] = {};
        subpassDescs[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescs[i].colorAttachmentCount = colorAttachmentReferences[i].size();
        subpassDescs[i].pColorAttachments = colorAttachmentReferences[i].data();
        subpassDescs[i].pDepthStencilAttachment = createInfo.subpasses[i]->depthAttachment.index == -1 ? nullptr : &depthAttachmentReferences[i];
        subpassDescs[i].inputAttachmentCount = inputAttachmentReferences[i].size();
        subpassDescs[i].pInputAttachments = inputAttachmentReferences[i].data();
    }

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachmentDescriptions.size();
    renderPassInfo.pAttachments = attachmentDescriptions.data();
    renderPassInfo.subpassCount = subpassDescs.size();
    renderPassInfo.pSubpasses = subpassDescs.data();
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

    VK_CHECK_RESULT(vkCreateRenderPass(g_vulkan_device->device(), &renderPassInfo, nullptr, &_renderPass));
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(g_vulkan_device->device(), _renderPass, nullptr);
}

} //namespace vulkan

} //namespace lv
