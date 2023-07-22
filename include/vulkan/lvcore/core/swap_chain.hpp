#ifndef LV_VULKAN_SWAP_CHAIN_H
#define LV_VULKAN_SWAP_CHAIN_H

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "framebuffer.hpp"
#include "device.hpp"
#include "command_buffer.hpp"

namespace lv {

struct Vulkan_SwapChainCreateInfo {
	LvndWindow* window;
  	bool vsyncEnable = true;
	uint8_t maxFramesInFlight = 2;
	bool clearAttachment = false;
	bool createDepthAttachment = false;
};

class Vulkan_SwapChain {
private:
	bool vsyncEnable;

	uint8_t _maxFramesInFlight;

	uint8_t _crntFrame = 0;
	uint32_t _imageIndex = 0;

	LvndWindow* _window;

	VkExtent2D swapChainExtent;

	Vulkan_Image* depthImage; //TODO: create depth image if required

	Vulkan_Subpass* subpass;
	Vulkan_RenderPass* _renderPass;
	Vulkan_Framebuffer* _framebuffer;
	Vulkan_CommandBuffer* _commandBuffer;
	Vulkan_Image* image;

	AttachmentLoadOperation loadOp = AttachmentLoadOperation::DontCare;

	VkExtent2D windowExtent;

	VkSwapchainKHR swapChain;
	VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;

	Vulkan_Semaphore* imageAvailableSemaphore;
	Vulkan_Semaphore* renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

public:
	Vulkan_SwapChain(Vulkan_SwapChainCreateInfo createInfo);

	~Vulkan_SwapChain();

	void destroyToResize();

	void create();

	void resize();

	void renderAndPresent();

	//Vulkan_SwapChain(const Vulkan_SwapChain &) = delete;
	//void operator=(const Vulkan_SwapChain &) = delete;

	size_t imageCount() { return image->frameCount(); }
	VkExtent2D getSwapChainExtent() { return swapChainExtent; }
	uint32_t width() { return swapChainExtent.width; }
	uint32_t height() { return swapChainExtent.height; }

	float extentAspectRatio() {
		return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
	}

	void acquireNextImage(/*uint32_t *imageIndex*/);
	VkResult submitCommandBuffers(const VkCommandBuffer *buffers/*, uint32_t *imageIndex*/);

	void createSwapChain();
	void createSyncObjects();

	// Helper functions
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

	//Getters
	inline uint8_t maxFramesInFlight() { return _maxFramesInFlight; }

	inline uint8_t crntFrame() { return _crntFrame; }

	inline uint32_t imageIndex() { return _imageIndex; }

	inline Vulkan_RenderPass* renderPass() { return _renderPass; }

	inline Vulkan_Framebuffer* framebuffer() { return _framebuffer; }

	inline Vulkan_CommandBuffer* commandBuffer() { return _commandBuffer; }
};

extern Vulkan_SwapChain* g_vulkan_swapChain;

//Implementation
//class Swap { public: static SwapChain* g_metal_swapChain; };

} //namespace lv

#endif
