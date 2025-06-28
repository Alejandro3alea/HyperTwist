set(TARGET_NAME Engine)

file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(${TARGET_NAME} STATIC ${ALL_SOURCES})


# Linux config
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Setting up libraries for Linux in ${TARGET_NAME} target...")
    target_link_directories(${TARGET_NAME} PRIVATE /usr/lib)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            GL
            GLEW
            freetype
            SDL3::SDL3
            SDL3_mixer::SDL3_mixer
    )

else() # Windows
    message("Setting up libraries for Windows in ${TARGET_NAME} target...")
    
    target_link_libraries(${TARGET_NAME}
        PUBLIC
            SDL3::SDL3
            SDL3_mixer::SDL3_mixer
    )

endif()


target_compile_definitions(${TARGET_NAME} PUBLIC SFML_STATIC)

target_compile_definitions(${TARGET_NAME} PRIVATE ENGINE_EXPORTS)
