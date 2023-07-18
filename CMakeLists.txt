cmake_minimum_required(VERSION 3.10)

project(lava_core)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
    -O3
    -Wno-nullability-completeness -Wno-deprecated-volatile
    -fPIC
)

if (UNIX AND NOT APPLE)
    add_compile_options(
        -ldl -lpthread -lX11
    )
endif ()

include_directories(
    "/opt/homebrew/Cellar/glm/0.9.9.8/include"
    "/Users/samuliak/Documents/lava_window/include"
    "external/vma"
    "external/stb/include"
    "external/nv_dds"
    "external/gli"
    "external/json/include"
    "external/glad/include"
    "include"
)

find_library(
    LVND_LIB
    NAMES lvnd liblvnd
    HINTS "/Users/samuliak/Documents/lava_window/lib"
    NO_DEFAULT_PATH
)

find_package(Vulkan REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(examples)
