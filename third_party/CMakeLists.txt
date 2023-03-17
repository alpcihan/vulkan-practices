set(THIRD_PARTY_LIB_NAME "third_party" CACHE STRING "Third Party Library Interface Name")
add_library(${THIRD_PARTY_LIB_NAME} INTERFACE)

set(VULKAN_PATH "C:/VulkanSDK/1.3.236.0") # TODO: make it automatic

target_include_directories(${THIRD_PARTY_LIB_NAME}
    INTERFACE 
        ${VULKAN_PATH}/Include/
)

add_subdirectory(glfw)
add_subdirectory(glm)

target_link_directories(${THIRD_PARTY_LIB_NAME}
    INTERFACE
        ${VULKAN_PATH}/Bin
        ${VULKAN_PATH}/Lib
)

target_link_libraries(${THIRD_PARTY_LIB_NAME} 
    INTERFACE 
        vulkan-1
        glfw
        glm
)