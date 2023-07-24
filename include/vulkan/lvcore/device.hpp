#ifndef LV_VULKAN_DEVICE_H
#define LV_VULKAN_DEVICE_H

#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <map>

#include <vk_mem_alloc.h>

#define LVND_BACKEND_VULKAN
#include "lvnd/lvnd.h"

#include "core.hpp"

#include "instance.hpp"

#include "lvcore/threading/thread_pool.hpp"

namespace lv {

namespace vulkan {

//TODO: use contructors and destructors
class DescriptorPool {
public:
    VkDescriptorPool descriptorPool;

    std::vector<VkDescriptorPool> oldPools;

    std::vector<VkDescriptorPoolSize> poolSizesVec{};
    uint32_t maxSets = 1024;
    VkDescriptorPoolCreateFlags poolFlags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; //TODO: use these flags?

    std::map<DescriptorType, uint32_t> poolSizesBegin;
    std::map<DescriptorType, uint32_t> poolSizes;

    void init(uint32_t maxSets, std::map<DescriptorType, uint32_t>& poolSizes);

    void create();

    void destroy();

    //void addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    
    void allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
    
    //void freeDescriptorSets(std::vector<VkDescriptorSet> &descriptors) const;
    
    //void resetPool();

    void recreate();
};

extern DescriptorPool* g_vulkan_descriptorPool;

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	uint32_t graphicsFamily;
	uint32_t presentFamily;
	bool graphicsFamilyHasValue = false;
	bool presentFamilyHasValue = false;
	bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

struct DeviceCreateInfo {
    LvndWindow* window;
    ThreadPool* threadPool;
    uint32_t maxDescriptorSets = 1024;
	std::map<DescriptorType, uint32_t> descriptorPoolSizes = {
		{DescriptorType::UniformBuffer, 128},
    	{DescriptorType::CombinedImageSampler, 512},
        {DescriptorType::SampledImage, 64},
        {DescriptorType::StorageImage, 16},
        {DescriptorType::InputAttachment, 16},
        {DescriptorType::StorageBuffer, 16}
	};
};

class Device {
private:
    uint8_t maxThreadCount;

    DescriptorPool descriptorPool;

    VkPhysicalDeviceProperties properties;

    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    std::vector<VkCommandPool> commandPools;

    VmaAllocator _allocator;

    VkDevice _device;
    VkSurfaceKHR _surface;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;

public:
    Device(DeviceCreateInfo createInfo);

    ~Device();

    void waitIdle() { vkDeviceWaitIdle(_device); }

    SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(_physicalDevice); }
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(_physicalDevice); }
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkCommandBuffer beginSingleTimeCommands(uint8_t threadIndex);
    void endSingleTimeCommands(uint8_t threadIndex, VkCommandBuffer commandBuffer);

    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool(uint8_t index);

    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME, VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME/*, "VK_KHR_portability_subset"*/};

    //Getters
    inline VkDevice device() { return _device; }

    inline VkPhysicalDevice physicalDevice() { return _physicalDevice; }

    inline VkCommandPool commandPool(uint8_t threadIndex) { return commandPools[threadIndex]; }

    inline VmaAllocator allocator() { return _allocator; }

    inline VkSurfaceKHR surface() { return _surface; }

    inline VkQueue graphicsQueue() { return _graphicsQueue; }

    inline VkQueue presentQueue() { return _presentQueue; }
};

extern Device* g_vulkan_device;

} //namespace vulkan

} //namespace lv

#endif
