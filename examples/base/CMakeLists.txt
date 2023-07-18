cmake_minimum_required(VERSION 3.10)

add_library(EXAMPLE_BASE_LIB
    application.cpp
    model.cpp
    camera.cpp
    first_person_camera.cpp
    frustum.cpp
)

target_link_libraries(EXAMPLE_BASE_LIB PUBLIC
    ${Vulkan_LIBRARIES}
)
