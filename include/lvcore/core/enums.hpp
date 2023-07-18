#ifndef LV_ENUMS_H
#define LV_ENUMS_H

#ifdef LV_BACKEND_VULKAN

#include "vulkan/lvcore/core/enums.hpp"

#endif

#ifdef LV_BACKEND_METAL

#include "metal/lvcore/core/enums.hpp"

#endif

#ifdef LV_BACKEND_OPENGL

#include "opengl/lvcore/core/enums.hpp"

#endif

#endif
