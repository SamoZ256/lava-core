#ifndef LV_VULKAN_INSTANCE_H
#define LV_VULKAN_INSTANCE_H

#include "lvcore/internal/instance.hpp"

#include <vulkan/vulkan.h>

namespace lv {

namespace vulkan {

class Instance : public internal::Instance {
private:
    VkInstance _instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    uint32_t _vulkanVersion;

    static const std::vector<const char*> validationLayers;

public:
    Instance(internal::InstanceCreateInfo createInfo);

    ~Instance();

    void setupDebugMessenger();
    bool checkValidationLayerSupport();
    void hasGflwRequiredInstanceExtensions();
    std::vector<const char *> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    //Getters
    inline VkInstance instance() { return _instance; }

    inline uint32_t vulkanVersion() { return _vulkanVersion; }

    inline size_t validationLayerCount() { return validationLayers.size(); }

    inline auto validationLayersData() { return validationLayers.data(); }
};

extern Instance* g_vulkan_instance;

} //namespace vulkan

} //namespace lv

#endif
