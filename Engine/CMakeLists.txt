set(TARGET_NAME Engine)

file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(${TARGET_NAME} STATIC ${ALL_SOURCES})

# Linux config
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Setting up libraries for Linux in ${TARGET_NAME} target...")
else() # Windows
    message("Setting up libraries for Windows in ${TARGET_NAME} target...")
endif()

message(${SFML_LIB_DIR})
target_link_directories(${TARGET_NAME}
    PUBLIC  $<$<CONFIG:Debug>:${SFML_LIB_DIR}/Debug>
            $<$<CONFIG:Release>:${SFML_LIB_DIR}/Release>
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        $<$<CONFIG:Debug>:libsfml-main-s-d.a>
        $<$<CONFIG:Debug>:libsfml-system-d.a>
        $<$<CONFIG:Debug>:libsfml-audio-d.a>
        
        $<$<CONFIG:Release>:libsfml-main-s.a>
        $<$<CONFIG:Release>:libsfml-system.a>
        $<$<CONFIG:Release>:libsfml-audio.a>
)

target_compile_definitions(${TARGET_NAME} PUBLIC SFML_STATIC)
target_link_options(${TARGET_NAME} PUBLIC -static-libgcc -static-libstdc++)

target_compile_definitions(${TARGET_NAME} PRIVATE ENGINE_EXPORTS)
