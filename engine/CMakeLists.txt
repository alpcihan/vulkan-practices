set(ENGINE_LIB_NAME "engine")

add_library(${ENGINE_LIB_NAME})

target_include_directories(${ENGINE_LIB_NAME}
    PRIVATE ./
)

file(GLOB_RECURSE ENGINE_SOURCE
    "./*"
)

target_sources(${ENGINE_LIB_NAME} 
    PRIVATE
        ${ENGINE_SOURCE}
)

target_link_libraries(${ENGINE_LIB_NAME} 
    PUBLIC
        ${THIRD_PARTY_LIB_NAME} # TODO: make it private
)