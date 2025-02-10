file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(Engine STATIC ${ALL_SOURCES})

target_link_directories(Engine
    PUBLIC  $<$<CONFIG:Debug>:${SFML_LIB_DIR}/Debug>
            $<$<CONFIG:Release>:${SFML_LIB_DIR}/Release>
)

target_link_libraries(Engine
    PUBLIC
        $<$<CONFIG:Debug>:libsfml-main-s-d.a>
        $<$<CONFIG:Debug>:libsfml-system-d.a>
        $<$<CONFIG:Debug>:libsfml-audio-d.a>
        
        $<$<CONFIG:Release>:libsfml-main-s.a>
        $<$<CONFIG:Release>:libsfml-system.a>
        $<$<CONFIG:Release>:libsfml-audio.a>
)

target_compile_definitions(Engine PUBLIC SFML_STATIC)
target_link_options(Engine PUBLIC -static-libgcc -static-libstdc++)

target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
