file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Game ${ALL_SOURCES})

option(TEST_MODE "Enable test accounts" ON)
if(TEST_MODE)
    add_compile_definitions(TEST_MODE)
endif()

target_include_directories(Game
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_directories(Game
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/AL
            ${CMAKE_SOURCE_DIR}/lib/GL
            ${CMAKE_SOURCE_DIR}/lib/SDL2
            ${CMAKE_SOURCE_DIR}/lib/freetype
            $<$<CONFIG:Debug>:${SFML_LIB_DIR}/Debug>
            $<$<CONFIG:Release>:${SFML_LIB_DIR}/Release>
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
        $<$<CONFIG:Debug>:libsfml-main-s-d.a>
        $<$<CONFIG:Debug>:libsfml-system-d.a>
        $<$<CONFIG:Debug>:libsfml-audio-d.a>
        
        $<$<CONFIG:Release>:libsfml-main-s.a>
        $<$<CONFIG:Release>:libsfml-system.a>
        $<$<CONFIG:Release>:libsfml-audio.a>
)

set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)


# Define a function that adds post-build copy commands for a given target
function(copy_required_files target file)
  # Copy the data folder only for Release builds
  add_custom_command(TARGET ${target} POST_BUILD
    CONFIGURATIONS Release
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/${target}/data"
      "$<TARGET_FILE_DIR:${target}>/data"
    COMMENT "Copying data folder..."
  )

  # Copy a DLL file (or multiple DLLs) only for Release builds
  add_custom_command(TARGET ${target} POST_BUILD
    CONFIGURATIONS Release
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/Game/${target}"
      "$<TARGET_FILE_DIR:${target}>"
    COMMENT "Copying ${file}..."
  )
endfunction()

# Define a function that adds post-build copy commands for a given target
function(copy_required_files target)
  # Copy the data folder only for Release builds
  add_custom_command(TARGET ${target} POST_BUILD
    CONFIGURATIONS Release
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/data"
      "$<TARGET_FILE_DIR:${target}>/data"
    COMMENT "Copying data folder for Release build..."
  )

  # Copy a DLL file (or multiple DLLs) only for Release builds
  add_custom_command(TARGET ${target} POST_BUILD
    CONFIGURATIONS Release
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/dlls/yourlibrary.dll"
      "$<TARGET_FILE_DIR:${target}>"
    COMMENT "Copying DLL for Release build..."
  )
endfunction()