#ifndef LV_METAL_DEVICE_H
#define LV_METAL_DEVICE_H

#include <map>

#include "lvnd/lvnd.h"

#include "lvcore/core/core.hpp"

#include "common.hpp"

#include "lvcore/threading/thread_pool.hpp"

namespace lv {

struct Metal_DeviceCreateInfo {
    LvndWindow* window;
    ThreadPool* threadPool;
    uint32_t maxDescriptorSets = 1024;
	std::map<DescriptorType, uint16_t> descriptorPoolSizes = {
		{DescriptorType::UniformBuffer, 4},
    	{DescriptorType::CombinedImageSampler, 4}
	};
};

class Metal_Device {
private:
    id /*MTLDevice*/ _device;
    id /*MTLCommandQueue*/ _commandQueue;

public:
    Metal_Device(Metal_DeviceCreateInfo createInfo);

    ~Metal_Device();

    void waitIdle() {}

    //Getters
    inline id /*MTLDevice*/ device() { return _device; }

    inline id /*MTLCommandQueue*/ commandQueue() { return _commandQueue; }
};

extern Metal_Device* g_metal_device;

} //namespace lv

#endif
