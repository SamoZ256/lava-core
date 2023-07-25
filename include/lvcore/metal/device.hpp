#ifndef LV_METAL_DEVICE_H
#define LV_METAL_DEVICE_H

#include "lvcore/internal/device.hpp"

#include "common.hpp"

namespace lv {

namespace metal {

class Device : public internal::Device {
private:
    id /*MTLDevice*/ _device;
    id /*MTLCommandQueue*/ _commandQueue;

public:
    Device(internal::DeviceCreateInfo createInfo);

    ~Device() override;

    void waitIdle() override {}

    //Getters
    inline id /*MTLDevice*/ device() { return _device; }

    inline id /*MTLCommandQueue*/ commandQueue() { return _commandQueue; }
};

extern Device* g_metal_device;

} //namespace metal

} //namespace lv

#endif
