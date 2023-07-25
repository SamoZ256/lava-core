#include "lvcore/metal/instance.hpp"

namespace lv {

namespace metal {

//Implementation
Instance* g_metal_instance = nullptr;

Instance::Instance(internal::InstanceCreateInfo createInfo) {
    renderAPI = createInfo.renderAPI;
    validationEnable = createInfo.validationEnable;

    g_metal_instance = this;
}

} //namespace metal

} //namespace lv
