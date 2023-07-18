#ifndef LV_VULKAN_INSTANCE_H
#define LV_VULKAN_INSTANCE_H

#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

#include <vulkan/vulkan.h>

#define LV_MAKE_VERSION VK_API_MAKE_VERSION

#define LV_VULKAN_VERSION_1_0 VK_API_VERSION_1_0
#define LV_VULKAN_VERSION_1_1 VK_API_VERSION_1_1
#define LV_VULKAN_VERSION_1_2 VK_API_VERSION_1_2
#define LV_VULKAN_VERSION_1_3 VK_API_VERSION_1_3

namespace lv {

struct Vulkan_InstanceCreateInfo {
    const char* applicationName;
    uint32_t applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    uint32_t vulkanVersion = VK_API_VERSION_1_0;
    bool validationEnable = false;
};

class Vulkan_Instance {
private:
    VkInstance _instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    uint32_t _vulkanVersion;
    bool _validationEnable;

    static const std::vector<const char*> validationLayers;

public:
    Vulkan_Instance(Vulkan_InstanceCreateInfo createInfo);

    ~Vulkan_Instance();

    void setupDebugMessenger();
    bool checkValidationLayerSupport();
    void hasGflwRequiredInstanceExtensions();
    std::vector<const char *> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    //Getters
    inline VkInstance instance() { return _instance; }

    inline uint32_t vulkanVersion() { return _vulkanVersion; }

    inline bool validationEnable() { return _validationEnable; }

    inline size_t validationLayerCount() { return validationLayers.size(); }

    inline auto validationLayersData() { return validationLayers.data(); }
};

extern Vulkan_Instance* g_vulkan_instance;

} //namespace lv

#endif
