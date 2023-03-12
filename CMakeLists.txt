cmake_minimum_required(VERSION 3.25.1)
set (CMAKE_CXX_STANDARD 20)

project(vulkan-practices)

include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(third_party)
add_subdirectory(engine)

add_executable(${CMAKE_PROJECT_NAME} vulkan_practices.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} 
    PRIVATE
        engine
)