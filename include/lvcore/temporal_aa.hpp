#ifndef LV_TEMPORAL_AA_H
#define LV_TEMPORAL_AA_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/temporal_aa.hpp"

namespace lv {

typedef Vulkan_TemporalAA TemporalAA;

} //namespace lv

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/temporal_aa.hpp"

namespace lv {

typedef Metal_TemporalAA TemporalAA;

} //namespace lv

#endif

#endif
