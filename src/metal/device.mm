#include "metal/lvcore/device.hpp"

namespace lv {

namespace metal {

Device* g_metal_device = nullptr;

Device::Device(internal::DeviceCreateInfo createInfo) {
    _device = MTLCreateSystemDefaultDevice();
    _commandQueue = [_device newCommandQueue];

    g_metal_device = this;
}

Device::~Device() {
    [_device release];
    [_commandQueue release];
}

} //namespace metal

} //namespace lv
