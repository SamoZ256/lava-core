#ifndef LV_METAL_DEVICE_H
#define LV_METAL_DEVICE_H

#include <map>

#include "lvnd/lvnd.h"

#include "common.hpp"

#include "enums.hpp"

#include "lvcore/threading/thread_pool.hpp"

namespace lv {

struct Metal_DeviceCreateInfo {
    LvndWindow* window;
    ThreadPool* threadPool;
    uint32_t maxDescriptorSets = 1024;
	std::map<LvDescriptorType, uint16_t> descriptorPoolSizes = {
		{LV_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4},
    	{LV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4}
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
