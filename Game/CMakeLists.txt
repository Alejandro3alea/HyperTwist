file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Game ${ALL_SOURCES})

option(TEST_MODE "Enable test accounts" ON)
if(TEST_MODE)
    add_compile_definitions(TEST_MODE)
endif()

target_include_directories(Game
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_directories(Game
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/AL
            ${CMAKE_SOURCE_DIR}/lib/GL
            ${CMAKE_SOURCE_DIR}/lib/SDL2
            ${CMAKE_SOURCE_DIR}/lib/freetype
            ${CMAKE_SOURCE_DIR}/lib/SFML/Release
            ${CMAKE_SOURCE_DIR}/lib/aubio
)

target_link_libraries(Game
    PRIVATE
        Engine
        OpenAL32.lib
        opengl32.lib
        glew32.lib
        SDL2.lib
        SDL2_mixer.lib
        freetype.lib
        aubio.dll.a
        libaubio.a
        sfml-main.lib
        sfml-system.lib
        sfml-audio.lib
)

# Set output directories for Debug and Release builds
set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)