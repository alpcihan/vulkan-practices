cmake_minimum_required(VERSION 3.25.1)
set (CMAKE_CXX_STANDARD 20)

project(vulkan-practices)

include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(third_party)
add_subdirectory(engine)

# compile shaders
set(WIN32_SHADER_COMPILE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders")
if (WIN32)
    execute_process(
        COMMAND
            "${WIN32_SHADER_COMPILE_DIR}/compile.bat"
        WORKING_DIRECTORY
            ${WIN32_SHADER_COMPILE_DIR}
    )
    if (result)
        message(FATAL_ERROR "Failed to compile shaders")
    endif()
endif()

add_executable(${CMAKE_PROJECT_NAME} vulkan_practices.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} 
    PRIVATE
        engine
)