#ifndef LV_INTERNAL_DEVICE_H
#define LV_INTERNAL_DEVICE_H

#include <map>

#include "lvnd/lvnd.h"

#include "common.hpp"

#include "lvcore/threading/thread_pool.hpp"

namespace lv {

namespace internal {

struct DeviceCreateInfo {
    LvndWindow* window;
    ThreadPool* threadPool;
    uint32_t maxDescriptorSets = 1024;
	std::map<DescriptorType, uint16_t> descriptorPoolSizes = {
		{DescriptorType::UniformBuffer, 4},
    	{DescriptorType::CombinedImageSampler, 4}
	};
};

class Device {
public:
    virtual ~Device() {}

    virtual void waitIdle() = 0;
};

} //namespace internal

} //namespace lv

#endif
