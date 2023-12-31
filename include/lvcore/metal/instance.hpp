#ifndef LV_METAL_INSTANCE_H
#define LV_METAL_INSTANCE_H

#include "lvcore/internal/instance.hpp"

namespace lv {

namespace metal {

class Instance : public internal::Instance {
public:
    Instance(internal::InstanceCreateInfo createInfo) {}

    ~Instance() override {}
};

} //namespace metal

} //namespace lv

#endif
