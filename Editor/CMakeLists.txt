file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Editor ${ALL_SOURCES})

target_include_directories(Editor
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Editor
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Editor/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_directories(Editor
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/AL
            ${CMAKE_SOURCE_DIR}/lib/GL
            ${CMAKE_SOURCE_DIR}/lib/SDL2
            ${CMAKE_SOURCE_DIR}/lib/freetype
            $<$<CONFIG:Debug>:${SFML_LIB_DIR}/Debug>
            $<$<CONFIG:Release>:${SFML_LIB_DIR}/Release>
            ${CMAKE_SOURCE_DIR}/lib/aubio
)

target_link_libraries(Editor
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
        $<$<CONFIG:Debug>:libsfml-main-s-d.a>
        $<$<CONFIG:Debug>:libsfml-system-d.a>
        $<$<CONFIG:Debug>:libsfml-audio-d.a>
        
        $<$<CONFIG:Release>:libsfml-main-s.a>
        $<$<CONFIG:Release>:libsfml-system.a>
        $<$<CONFIG:Release>:libsfml-audio.a>
)

set_target_properties(Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)