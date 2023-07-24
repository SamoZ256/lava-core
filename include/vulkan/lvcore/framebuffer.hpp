#ifndef LV_VULKAN_FRAMEBUFFER_H
#define LV_VULKAN_FRAMEBUFFER_H

#include "render_pass.hpp"

namespace lv {

namespace vulkan {

struct FramebufferCreateInfo {
    uint8_t frameCount = 0;
    RenderPass* renderPass;
    std::vector<FramebufferAttachment> colorAttachments;
    FramebufferAttachment depthAttachment;
};

class Framebuffer {
private:
    uint8_t _frameCount;

    std::vector<VkFramebuffer> framebuffers;
    RenderPass* _renderPass;

    uint16_t _width = 0, _height = 0;
    uint16_t maxLayerCount = 1;

    std::vector<VkClearValue> clearValues{};

public:
    Framebuffer(FramebufferCreateInfo createInfo);

    ~Framebuffer();

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline VkFramebuffer framebuffer(uint8_t index) { return framebuffers[index]; }

    inline RenderPass* renderPass() { return _renderPass; }

    inline uint16_t width() { return _width; }

    inline uint16_t height() { return _height; }

    inline size_t clearValueCount() { return clearValues.size(); }

    inline VkClearValue* clearValuesData() { return clearValues.data(); }
};

} //namespace vulkan

} //namespace lv

#endif
