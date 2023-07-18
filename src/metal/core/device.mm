#include "metal/lvcore/core/device.hpp"

namespace lv {

Metal_Device* g_metal_device = nullptr;

Metal_Device::Metal_Device(Metal_DeviceCreateInfo createInfo) {
    _device = MTLCreateSystemDefaultDevice();
    _commandQueue = [_device newCommandQueue];

    g_metal_device = this;
}

Metal_Device::~Metal_Device() {
    [_device release];
    [_commandQueue release];
}

} //namespace lv
