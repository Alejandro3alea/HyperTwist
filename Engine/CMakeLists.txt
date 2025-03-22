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
    )

else() # Windows
    message("Setting up libraries for Windows in ${TARGET_NAME} target...")

    target_link_directories(${TARGET_NAME}
    PUBLIC
            $<$<CONFIG:Debug>:${SFML_LIB_DIR}/Debug>
            $<$<CONFIG:Release>:${SFML_LIB_DIR}/Release>
    )

    target_link_libraries(${TARGET_NAME}
    PUBLIC
            $<$<CONFIG:Debug>:sfml-main-s-d>
            $<$<CONFIG:Release>:sfml-main-s>
            $<$<CONFIG:Debug>:sfml-system-d>
            $<$<CONFIG:Release>:sfml-system>
            $<$<CONFIG:Debug>:sfml-audio-d>
            $<$<CONFIG:Release>:sfml-audio>
    )
endif()


target_compile_definitions(${TARGET_NAME} PUBLIC SFML_STATIC)

target_compile_definitions(${TARGET_NAME} PRIVATE ENGINE_EXPORTS)
