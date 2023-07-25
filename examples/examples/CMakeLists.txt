include_directories(
    "/Users/samuliak/Documents/lava_window/include"
    "/Users/samuliak/Documents/lava_core/include"
    "../external/PerlinNoise"
    "../base"
    ${GLM_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

if(APPLE)
    find_library(NS_FOUNDATION_LIB Foundation)
    find_library(QUARTZCORE_LIB QuartzCore)
    find_library(UNIFORMTYPEIDENTIFIERS_LIB UniformTypeIdentifiers)
    find_library(APPKIT_LIB AppKit)
    find_library(METAL_LIB Metal)
endif()

function(ADD_EXAMPLE EXAMPLE_NAME)
    add_executable(${EXAMPLE_NAME}
        "${EXAMPLE_NAME}/main.cpp"
    )

    target_link_libraries(${EXAMPLE_NAME} PUBLIC
        ${LVND_LIB}
        ${ASSIMP_LIBRARIES}
        LAVA_CORE_LIB
        EXAMPLE_BASE_LIB
    )

    if (APPLE)
        target_link_libraries(${EXAMPLE_NAME} PUBLIC
            "-framework GameController"
            ${NS_FOUNDATION_LIB}
            ${QUARTZCORE_LIB}
            ${UNIFORMTYPEIDENTIFIERS_LIB}
            ${APPKIT_LIB}
            ${METAL_LIB}
        )
    endif ()

    target_link_libraries(${EXAMPLE_NAME} PUBLIC
        ${Vulkan_LIBRARIES}
    )
endfunction(ADD_EXAMPLE)

ADD_EXAMPLE("01-window")
ADD_EXAMPLE("02-clear_color")
ADD_EXAMPLE("03-triangle")
ADD_EXAMPLE("04-vertex_buffer")
ADD_EXAMPLE("05-index_buffer")
ADD_EXAMPLE("06-push_constants")
ADD_EXAMPLE("07-uniform_buffer")
ADD_EXAMPLE("08-texture")
ADD_EXAMPLE("09-model")
ADD_EXAMPLE("10-phong_shading")
ADD_EXAMPLE("compute_shader")
ADD_EXAMPLE("compute_and_instanced")
ADD_EXAMPLE("shadow_mapping")
ADD_EXAMPLE("point_light_shadows")
ADD_EXAMPLE("deferred_rendering")
ADD_EXAMPLE("deferred_and_hdr")
ADD_EXAMPLE("tile_based_deferred_rendering")
ADD_EXAMPLE("terrain_rendering")
ADD_EXAMPLE("modern_rendering")
