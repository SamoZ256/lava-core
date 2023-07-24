#ifndef LV_INTERNAL_DESCRIPTOR_SET_H
#define LV_INTERNAL_DESCRIPTOR_SET_H

#include "common.hpp"

#include "pipeline_layout.hpp"
#include "buffer.hpp"
#include "image.hpp"

namespace lv {

namespace internal {

struct DescriptorSetCreateInfo {
    uint8_t frameCount = 0;
    PipelineLayout* pipelineLayout;
    uint8_t layoutIndex = 0;
    std::vector<BufferDescriptorInfo*> bufferBindings;
    std::vector<ImageDescriptorInfo*> imageBindings;
};

class DescriptorSet {
protected:
    uint8_t _frameCount;

    uint8_t _layoutIndex;

public:
    virtual ~DescriptorSet() {}

    //Getters
    inline uint8_t frameCount() { return _frameCount; }

    inline uint8_t layoutIndex() { return _layoutIndex; }
};

} //namespace internal

} //namespace lv

#endif
