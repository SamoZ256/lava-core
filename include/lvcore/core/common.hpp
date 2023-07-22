#ifndef LV_COMMON_H
#define LV_COMMON_H

#include <stdio.h>

#include "core.hpp"

#define _LV_MSG(type, msg) fprintf(stderr, "[LV:" type "]::%s: %s\n", __FUNCTION__, msg)

#define LV_ERROR(msg) _LV_MSG("error", msg)

#define LV_WARN(msg) _LV_MSG("warning", msg)

#define LV_INVALID_ARGUMENT(argName) LV_ERROR("Invalid argument '" argName "'")

#define LV_CHECK_ARGUMENT(type, arg) \
if (arg < (type)0 || arg >= type::MaxEnum) \
    LV_INVALID_ARGUMENT(#arg)

#define LV_WARN_UNSUPPORTED(featureName) LV_WARN("'" featureName "' is unsupported")

#define LV_WARN_UNSUPPORTED_FUNCTION LV_WARN("This function is unsupported")

#endif
