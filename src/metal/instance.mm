#include "metal/lvcore/core/instance.hpp"

namespace lv {

//Implementation
Metal_Instance* g_metal_instance = nullptr;

Metal_Instance::Metal_Instance(Metal_InstanceCreateInfo createInfo) {
    validationEnable = createInfo.validationEnable;

    g_metal_instance = this;
}

} //namespace lv
