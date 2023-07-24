#include "vulkan/lvcore/device.hpp"

#include "vulkan/lvcore/common.hpp"

namespace lv {

namespace vulkan {

Device* g_vulkan_device = nullptr;
DescriptorPool* g_vulkan_descriptorPool = nullptr;

// *************** Descriptor Pool *********************

void DescriptorPool::init(uint32_t aMaxSets, std::map<DescriptorType, uint32_t>& aPoolSizes) {
	maxSets = aMaxSets;
	poolSizesBegin = aPoolSizes;
	poolSizes = aPoolSizes;

	for (const auto& [key, value] : poolSizes) {
		VkDescriptorType vkDescriptorType;
		GET_VK_DESCRIPTOR_TYPE(key, vkDescriptorType);

		poolSizesVec.push_back({vkDescriptorType, value/* * g_vulkan_swapChain->maxFramesInFlight()*/});
	}

	g_vulkan_descriptorPool = this;
}

void DescriptorPool::create() {
	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizesVec.size());
	descriptorPoolInfo.pPoolSizes = poolSizesVec.data();
	descriptorPoolInfo.maxSets = maxSets;
	descriptorPoolInfo.flags = poolFlags;

	VK_CHECK_RESULT(vkCreateDescriptorPool(g_vulkan_device->device(), &descriptorPoolInfo, nullptr, &descriptorPool));
}

void DescriptorPool::destroy() {
  	vkDestroyDescriptorPool(g_vulkan_device->device(), descriptorPool, nullptr);
}

void DescriptorPool::allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.pSetLayouts = &descriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(g_vulkan_device->device(), &allocInfo, &descriptor));
}

/*
void DescriptorPool::freeDescriptorSets(std::vector<VkDescriptorSet> &descriptors) const {
  vkFreeDescriptorSets(
      g_metal_device->device()(),
      descriptorPool,
      static_cast<uint32_t>(descriptors.size()),
      descriptors.data());
}
*/

/*
void DescriptorPool::resetPool() {
  vkResetDescriptorPool(g_metal_device->device()(), descriptorPool, 0);
}
*/

/*
void DescriptorPool::addPoolSize(VkDescriptorType descriptorType, uint32_t count) {
  poolSizes.push_back({descriptorType, count});
}
*/

void DescriptorPool::recreate() {
	std::cout << "Recreating descriptor pool" << std::endl;
	oldPools.push_back(descriptorPool);
	//descriptorPool = VK_NULL_HANDLE;
	poolSizes = poolSizesBegin;
	create();
}

// class member functions
Device::Device(DeviceCreateInfo createInfo) {
	descriptorPool.init(createInfo.maxDescriptorSets, createInfo.descriptorPoolSizes);

	maxThreadCount = createInfo.threadPool->maxThreadCount();
	//window.createWindowSurface(instance, &surface_);
	VK_CHECK_RESULT(lvndVulkanCreateWindowSurface(createInfo.window, g_vulkan_instance->instance(), &_surface))
	pickPhysicalDevice();
	createLogicalDevice();
	commandPools.resize(maxThreadCount);
	for (uint8_t i = 0; i < maxThreadCount; i++)
		createCommandPool(i);
	
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.vulkanApiVersion = g_vulkan_instance->vulkanVersion();
    allocatorCreateInfo.physicalDevice = _physicalDevice;
    allocatorCreateInfo.device = _device;
    allocatorCreateInfo.instance = g_vulkan_instance->instance();
    
    VK_CHECK_RESULT(vmaCreateAllocator(&allocatorCreateInfo, &_allocator));

	g_vulkan_device = this;

	descriptorPool.create();
}

Device::~Device() {
	descriptorPool.destroy();

	vmaDestroyAllocator(_allocator);

	for (uint8_t i = 0; i < maxThreadCount; i++)
		vkDestroyCommandPool(_device, commandPools[i], nullptr);
	vkDestroyDevice(_device, nullptr);

	vkDestroySurfaceKHR(g_vulkan_instance->instance(), _surface, nullptr);
}

void Device::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(g_vulkan_instance->instance(), &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}
	std::cout << "Device count: " << deviceCount << std::endl;
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(g_vulkan_instance->instance(), &deviceCount, devices.data());

	for (const auto &device : devices) {
		if (isDeviceSuitable(device)) {
			_physicalDevice = device;
			break;
		}
	}

	if (_physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable GPU");
	}

	vkGetPhysicalDeviceProperties(_physicalDevice, &properties);
	std::cout << "Physical device: " << properties.deviceName << std::endl;
	std::cout << "Max push constants size: " << properties.limits.maxPushConstantsSize << " bytes" << std::endl;
}

void Device::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.independentBlend = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	// might not really be necessary anymore because device specific validation layers
	// have been deprecated
	if (g_vulkan_instance->validationEnable()) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(g_vulkan_instance->validationLayerCount());
		createInfo.ppEnabledLayerNames = g_vulkan_instance->validationLayersData();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	VK_CHECK_RESULT(vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device));

	vkGetDeviceQueue(_device, indices.graphicsFamily, 0, &_graphicsQueue);
	vkGetDeviceQueue(_device, indices.presentFamily, 0, &_presentQueue);
}

void Device::createCommandPool(uint8_t index) {
	QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VK_CHECK_RESULT(vkCreateCommandPool(_device, &poolInfo, nullptr, &commandPools[index]));
}

bool Device::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
			indices.graphicsFamilyHasValue = true;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
			indices.presentFamilyHasValue = true;
		}
		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

VkFormat Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	throw std::runtime_error("Failed to find supported format");
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

VkCommandBuffer Device::beginSingleTimeCommands(uint8_t threadIndex) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPools[threadIndex];
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Device::endSingleTimeCommands(uint8_t threadIndex, VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(_graphicsQueue);

	vkFreeCommandBuffers(_device, commandPools[threadIndex], 1, &commandBuffer);
}

} //namespace vulkan

} //namespace lv
