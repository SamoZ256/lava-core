cmake_minimum_required(VERSION 3.10)

project(lava_core_examples)

if (APPLE)
    set(BACKEND_METAL ON)
else()
    set(BACKEND_VULKAN ON)
endif()

set(BACKEND)

if (BACKEND_VULKAN)
    set(BACKEND vulkan)

    add_compile_definitions(
        LV_BACKEND_VULKAN
    )

    include_directories(
        "../../external/vma"
    )
elseif (BACKEND_METAL)
    set(BACKEND metal)

    add_compile_definitions(
        LV_BACKEND_METAL
    )
endif ()

find_package(Assimp)

find_package(glm REQUIRED)

include_directories(
    "/Users/samuliak/Documents/lava_window/include"
    "/Users/samuliak/Documents/lava_core/include"
    ${GLM_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

add_subdirectory(base)
add_subdirectory(examples)
