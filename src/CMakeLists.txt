file(GLOB src
    lvcore.cpp
    vulkan/*.cpp
    filesystem/*.cpp
    threading/*.cpp
    libraries/*.cpp
)

add_library(LAVA_CORE_LIB SHARED
    ${src}
    ../external/nv_dds/nv_dds.cpp
)
set_target_properties(LAVA_CORE_LIB PROPERTIES SOVERSION 1)

target_link_libraries(LAVA_CORE_LIB PUBLIC
    ${LVND_LIB}
)

target_link_libraries(LAVA_CORE_LIB PUBLIC
    ${Vulkan_LIBRARIES}
)

if (APPLE)
    file(GLOB metalSrc
        metal/*.mm
    )

    target_sources(LAVA_CORE_LIB PRIVATE
        ${metalSrc}
    )

    find_library(NS_FOUNDATION_LIB Foundation)
    find_library(QUARTZCORE_LIB QuartzCore)
    find_library(METAL_LIB Metal)
    find_library(METAL_KIT_LIB MetalKit)
    find_library(METALFX_LIB MetalFX)
    find_library(MPS_LIB MetalPerformanceShaders)

    target_link_libraries(LAVA_CORE_LIB PRIVATE
        ${NS_FOUNDATION_LIB}
        ${QUARTZCORE_LIB}
        ${METAL_LIB}
        ${METAL_KIT_LIB}
        ${METALFX_LIB}
        ${MPS_LIB}
    )
endif ()
