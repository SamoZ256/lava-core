#ifndef LV_TEMPORAL_AA_H
#define LV_TEMPORAL_AA_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/temporal_aa.hpp"

namespace lv {

typedef vulkan::TemporalAA TemporalAA;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/temporal_aa.hpp"

namespace lv {

typedef metal::TemporalAA TemporalAA;

} //namespace lv

#endif

#endif
