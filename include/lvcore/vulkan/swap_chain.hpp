#ifndef LV_VULKAN_SWAP_CHAIN_H
#define LV_VULKAN_SWAP_CHAIN_H

#include "lvcore/internal/swap_chain.hpp"

#include "framebuffer.hpp"
#include "device.hpp"
#include "command_buffer.hpp"

namespace lv {

namespace vulkan {

class SwapChain : public internal::SwapChain {
private:
	Bool vsyncEnable;

	uint32_t _imageIndex = 0;

	LvndWindow* _window;

	VkExtent2D swapChainExtent;

	Image* depthImage; //TODO: create depth image if required

	Subpass* subpass;
	RenderPass* renderPass;
	Framebuffer* framebuffer;
	CommandBuffer* commandBuffer;
	Image* image;

	AttachmentLoadOperation loadOp = AttachmentLoadOperation::DontCare;

	VkExtent2D windowExtent;

	VkSwapchainKHR swapChain;
	VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;

	Semaphore* imageAvailableSemaphore;
	Semaphore* renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

public:
	SwapChain(internal::SwapChainCreateInfo createInfo);

	~SwapChain() override;

	void destroyToResize();

	void create();

	void resize() override;

	void renderAndPresent() override;

	//SwapChain(const SwapChain &) = delete;
	//void operator=(const SwapChain &) = delete;

	size_t imageCount() { return image->frameCount(); }
	VkExtent2D getSwapChainExtent() { return swapChainExtent; }
	uint32_t width() { return swapChainExtent.width; }
	uint32_t height() { return swapChainExtent.height; }

	float extentAspectRatio() {
		return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
	}

	void acquireNextImage(/*uint32_t *imageIndex*/) override;
	VkResult submitCommandBuffers(const VkCommandBuffer *buffers/*, uint32_t *imageIndex*/);

	void createSwapChain();
	void createSyncObjects();

	// Helper functions
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

	//Getters
	inline uint32_t imageIndex() { return _imageIndex; }

    internal::RenderPass* getRenderPass() override {
        return renderPass;
    }

    internal::Framebuffer* getFramebuffer() override {
        return framebuffer;
    }

    internal::CommandBuffer* getCommandBuffer() override {
        return commandBuffer;
    }
};

extern SwapChain* g_vulkan_swapChain;

} //namespace vulkan

} //namespace lv

#endif
