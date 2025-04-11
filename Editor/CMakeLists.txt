set(TARGET_NAME Editor)

file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${TARGET_NAME} ${ALL_SOURCES})

target_include_directories(${TARGET_NAME}
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET_NAME}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_directories(${TARGET_NAME}
            ${CMAKE_SOURCE_DIR}/lib/GL
            ${CMAKE_SOURCE_DIR}/lib/SDL2
            ${CMAKE_SOURCE_DIR}/lib/freetype
)


# Linux config
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Setting up libraries for Linux...")
    target_link_directories(${TARGET_NAME} PRIVATE /usr/lib)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Engine
            GL
            GLEW
            SDL2
            SDL2_mixer
            freetype
    )
else() # Windows
    message("Setting up libraries for Windows...")
    target_link_libraries(${TARGET_NAME}
        PRIVATE 
            Engine
            opengl32.lib
            glew32.lib
            SDL2.lib
            SDL2_mixer.lib
            freetype.lib
    )
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)