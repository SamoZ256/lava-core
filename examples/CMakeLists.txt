cmake_minimum_required(VERSION 3.10)

project(lava_core_examples)

include_directories(
    "../../external/vma"
)

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
