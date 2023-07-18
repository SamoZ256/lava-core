#ifndef LV_METAL_INSTANCE_H
#define LV_METAL_INSTANCE_H

#include <stdint.h>

#define LV_MAKE_VERSION(variant, major, minor, patch) 0

#define LV_VULKAN_VERSION_1_0 0
#define LV_VULKAN_VERSION_1_1 0
#define LV_VULKAN_VERSION_1_2 0
#define LV_VULKAN_VERSION_1_3 0

namespace lv {

struct Metal_InstanceCreateInfo {
    const char* applicationName;
    uint32_t applicationVersion = LV_MAKE_VERSION(0, 1, 0, 0);
    uint32_t vulkanVersion = LV_VULKAN_VERSION_1_0;
    bool validationEnable = false;
};

class Metal_Instance {
private:
    bool validationEnable;

public:
    Metal_Instance(Metal_InstanceCreateInfo createInfo);

    ~Metal_Instance() {}
};

extern Metal_Instance* g_metal_instance;

} //namespace lv

#endif
