set(TARGET_NAME Game)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${TARGET_NAME} ${ALL_SOURCES}
        src/UI/UIElementData.h
        src/UI/UITableException.h)

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
            sfml-audio
            sfml-system
    )
else() # Windows
    message("Setting up libraries for Windows in ${TARGET_NAME} target...")
    message("${CMAKE_SOURCE_DIR}")

    target_link_libraries(${TARGET_NAME}
        PRIVATE 
            Engine
            OpenAL32.lib
            opengl32.lib
            glew32.lib
            SDL2.lib
            SDL2_mixer.lib
            freetype.lib
            aubio.dll.a.lib
            libaubio.a.lib

            $<$<CONFIG:Debug>:sfml-main-s-d.lib>
            $<$<CONFIG:Release>:sfml-main-s.lib>
            $<$<CONFIG:Debug>:sfml-system-d.lib>
            $<$<CONFIG:Release>:sfml-system.lib>
            $<$<CONFIG:Debug>:sfml-audio-d.lib>
            $<$<CONFIG:Release>:sfml-audio.lib>
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

copy_required_files(Game)

message("Exporting data folder to ${CMAKE_BINARY_DIR}")
if (EXPORT_BUILD)
    file(COPY "${CMAKE_SOURCE_DIR}/${TARGET_NAME}/data" DESTINATION "${CMAKE_BINARY_DIR}")
    set(DATA_PATH "${CMAKE_BINARY_DIR}/data")
else()
    set(DATA_PATH "${CMAKE_SOURCE_DIR}/Game/data")
endif()
target_compile_definitions(Engine PUBLIC DATA_PATH="${DATA_PATH}")
target_compile_definitions(${TARGET_NAME} PUBLIC DATA_PATH="${DATA_PATH}")
