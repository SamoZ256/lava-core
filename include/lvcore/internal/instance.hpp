#ifndef LV_INTERNAL_INSTANCE_H
#define LV_INTERNAL_INSTANCE_H

#include "common.hpp"

namespace lv {

namespace internal {

struct InstanceCreateInfo {
    RenderAPI renderAPI = defaultRenderAPI;
    const char* applicationName;
    Bool validationEnable = False;
};

class Instance {
protected:
    RenderAPI renderAPI;
    Bool validationEnable;

public:
    virtual ~Instance() {}

    //Getters
    inline RenderAPI getRenderAPI() { return renderAPI; }

    inline Bool getValidationEnable() { return validationEnable; }
};

} //namespace internal

} //namespace lv

#endif
