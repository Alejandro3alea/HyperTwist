set(TARGET_NAME Game)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${TARGET_NAME} ${ALL_SOURCES})

option(TEST_MODE "Enable test accounts" ON)
if(TEST_MODE)
    add_compile_definitions(TEST_MODE)
endif()

target_include_directories(${TARGET_NAME}
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_directories(${TARGET_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/AL
            ${CMAKE_SOURCE_DIR}/lib/GL
            ${CMAKE_SOURCE_DIR}/lib/SDL2
            ${CMAKE_SOURCE_DIR}/lib/freetype
            $<$<CONFIG:Debug>:${SFML_LIB_DIR}/Debug>
            $<$<CONFIG:Release>:${SFML_LIB_DIR}/Release>
            ${CMAKE_SOURCE_DIR}/lib/aubio
)


# Linux config
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Setting up libraries for Linux in ${TARGET_NAME} target...")
    target_link_directories(${TARGET_NAME} PRIVATE /usr/lib)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Engine
            openal
            GL
            GLEW
            SDL2
            SDL2_mixer
            freetype
            aubio
            $<$<CONFIG:Debug>:libsfml-main-s-d.a>
            $<$<CONFIG:Debug>:libsfml-system-d.a>
            $<$<CONFIG:Debug>:libsfml-audio-d.a>
            
            $<$<CONFIG:Release>:libsfml-main-s.a>
            $<$<CONFIG:Release>:libsfml-system.a>
            $<$<CONFIG:Release>:libsfml-audio.a>
    )
else() # Windows
    message("Setting up libraries for Windows in ${TARGET_NAME} target...")
    target_link_libraries(${TARGET_NAME}
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
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)


function(copy_dll target file)
  add_custom_command(TARGET ${target} POST_BUILD
    CONFIGURATIONS ${TARGET_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_SOURCE_DIR}/${TARGET_NAME}/${file}"
      "$<TARGET_FILE_DIR:${target}>/${file}"
    COMMENT "Copying ${file}..."
  )
endfunction()

function(copy_required_files target)
  copy_dll(${target} freetype.dll)
  copy_dll(${target} glew32.dll)
  copy_dll(${target} libaubio-5.dll)
  copy_dll(${target} SDL2_mixer.dll)
  copy_dll(${target} SDL2.dll)
  copy_dll(${target} sfml-audio-d-3.dll)
  copy_dll(${target} sfml-system-d-3.dll)
endfunction()

function(echo_path target file )
  message(STATUS "Configurations: ${configuration}") # Debug line
  add_custom_command(TARGET ${target} POST_BUILD
    CONFIGURATIONS ${TARGET_NAME}
    COMMAND ${CMAKE_COMMAND} -E echo "Path for ${file}: "
    COMMENT "Echoing path for ${file}..."
  )
endfunction()

copy_required_files(Game)