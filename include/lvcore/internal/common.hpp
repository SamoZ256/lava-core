#ifndef LV_COMMON_H
#define LV_COMMON_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include "core.hpp"

#define CAST_FROM_INTERNAL(name, type) type* name##_ = static_cast<type*>(name);

#define CAST_FROM_INTERNAL_NAMED(name, type, newName) type* newName = static_cast<type*>(name);

//Errors and warnings
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
