#ifndef LV_INTERNAL_DEVICE_H
#define LV_INTERNAL_DEVICE_H

#include <map>

#define LVND_BACKEND_VULKAN
#ifdef __APPLE__
#define LVND_BACKEND_METAL
#endif
#include "lvnd/lvnd.h"

#include "common.hpp"

#include "lvcore/threading/thread_pool.hpp"

namespace lv {

namespace internal {

struct DeviceCreateInfo {
    LvndWindow* window;
    ThreadPool* threadPool;
    uint32_t maxDescriptorSets = 1024;
	std::map<DescriptorType, uint32_t> descriptorPoolSizes = {
		{DescriptorType::UniformBuffer, 128},
    	{DescriptorType::CombinedImageSampler, 512},
        {DescriptorType::SampledImage, 64},
        {DescriptorType::StorageImage, 16},
        {DescriptorType::InputAttachment, 16},
        {DescriptorType::StorageBuffer, 16}
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
