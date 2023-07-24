#ifndef LV_INTERNAL_INSTANCE_H
#define LV_INTERNAL_INSTANCE_H

#include "common.hpp"

namespace lv {

namespace internal {

struct InstanceCreateInfo {
    const char* applicationName;
    Bool validationEnable = False;
};

class Instance {
protected:
    Bool validationEnable;

public:
    virtual ~Instance() {}

    //Getters
    inline Bool getValidationEnable() { return validationEnable; }
};

} //namespace internal

} //namespace lv

#endif
