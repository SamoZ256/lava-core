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

#include "enums.hpp"

#include "instance.hpp"

#include "lvcore/threading/thread_pool.hpp"

namespace lv {

class Vulkan_DescriptorPool {
public:
    VkDescriptorPool descriptorPool;

    std::vector<VkDescriptorPool> oldPools;

    std::vector<VkDescriptorPoolSize> poolSizesVec{};
    uint32_t maxSets = 1024;
    VkDescriptorPoolCreateFlags poolFlags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    std::map<LvDescriptorType, uint32_t> poolSizesBegin;
    std::map<LvDescriptorType, uint32_t> poolSizes;

    void init(uint32_t maxSets, std::map<LvDescriptorType, uint32_t>& poolSizes);

    void create();

    void destroy();

    //void addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    
    void allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
    
    //void freeDescriptorSets(std::vector<VkDescriptorSet> &descriptors) const;
    
    //void resetPool();

    void recreate();
};

extern Vulkan_DescriptorPool* g_vulkan_descriptorPool;

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

struct Vulkan_DeviceCreateInfo {
    LvndWindow* window;
    ThreadPool* threadPool;
    uint32_t maxDescriptorSets = 1024;
	std::map<VkDescriptorType, uint32_t> descriptorPoolSizes = {
		{LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 128},
    	{LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 512},
        {LV_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 64},
        {LV_DESCRIPTOR_TYPE_STORAGE_IMAGE, 16},
        {LV_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 16},
        {LV_DESCRIPTOR_TYPE_STORAGE_BUFFER, 16}
	};
};

class Vulkan_Device {
private:
    uint8_t maxThreadCount;

    Vulkan_DescriptorPool descriptorPool;

    VkPhysicalDeviceProperties properties;

    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    std::vector<VkCommandPool> commandPools;

    VmaAllocator _allocator;

    VkDevice _device;
    VkSurfaceKHR _surface;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;

public:
    Vulkan_Device(Vulkan_DeviceCreateInfo createInfo);

    ~Vulkan_Device();

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

extern Vulkan_Device* g_vulkan_device;

//Implementation
//class Dev { public: static Device* g_metal_device; };

} //namespace lv

#endif
