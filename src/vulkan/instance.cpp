#include "lvcore/vulkan/instance.hpp"

#define LVND_BACKEND_VULKAN
#include "lvnd/lvnd.h"

#include "lvcore/vulkan/common.hpp"

namespace lv {

namespace vulkan {

//Implementation
Instance* g_vulkan_instance = nullptr;
const std::vector<const char *> Instance::validationLayers = {"VK_LAYER_KHRONOS_validation"};

// local callback functions
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

Instance::Instance(internal::InstanceCreateInfo createInfo) {
    renderAPI = createInfo.renderAPI;
	validationEnable = createInfo.validationEnable;
	
	_vulkanVersion = VK_API_VERSION_1_0;
    if (validationEnable && !checkValidationLayerSupport()) {
		throw std::runtime_error("Validation layers requested, but not available");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = createInfo.applicationName;
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.pEngineName = "Lava Engine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.apiVersion = _vulkanVersion;

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
#ifdef __APPLE__
	instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	auto extensions = getRequiredExtensions();
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (validationEnable) {
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	} else {
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;
	}

	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &_instance))

	hasGflwRequiredInstanceExtensions();
	setupDebugMessenger();

	g_vulkan_instance = this;
}

Instance::~Instance() {
	if (validationEnable)
		DestroyDebugUtilsMessengerEXT(_instance, debugMessenger, nullptr);

	vkDestroyInstance(_instance, nullptr);
}

void Instance::setupDebugMessenger() {
	if (validationEnable) {
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);
		VK_CHECK_RESULT(CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &debugMessenger));
	}
}

bool Instance::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	for (const char *layerName : validationLayers) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	return true;
}

void Instance::hasGflwRequiredInstanceExtensions() {
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "Available extensions:" << std::endl;
	std::unordered_set<std::string> available;
	for (const auto &extension : extensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
		available.insert(extension.extensionName);
	}

	std::cout << "Required extensions:" << std::endl;
	auto requiredExtensions = getRequiredExtensions();
	for (const auto &required : requiredExtensions) {
		std::cout << "\t" << required << std::endl;
		if (available.find(required) == available.end()) {
			throw std::runtime_error("Missing required glfw extension");
		}
	}
}

void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;  // Optional
}

std::vector<const char *> Instance::getRequiredExtensions() {
	uint16_t lvndExtensionCount = 0;
	const char** lvndExtensions;
	lvndExtensions = lvndVulkanGetRequiredInstanceExtensions(&lvndExtensionCount);

	std::vector<const char*> extensions(lvndExtensions, lvndExtensions + lvndExtensionCount);
	/*
	for (uint8_t i = 0; i < extensions.size(); i++) {
		std::cout << "GLFWextension[" << (int)i << "]: " << extensions[i] << std::endl;
	}
	*/
	//TODO: query for support instead
#ifdef __APPLE__
	extensions.push_back("VK_KHR_portability_enumeration");
#endif

	if (validationEnable)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

} //namespace vulkan

} //namespace lv
