cmake_minimum_required(VERSION 3.25)
project(HyperTwist VERSION 0.1.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_GAME "Build the game executable" OFF)
option(BUILD_EDITOR "Build the editor executable" OFF)

# Prefer GLVND over the legacy OpenGL libraries.
set(OpenGL_GL_PREFERENCE GLVND)

# Linux config
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories(/usr/include)
    find_package(SFML 2.5 COMPONENTS system audio REQUIRED)
else()
    set(SFML_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/SFML")
    set(SFML_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/SFML")
endif()

# External includes
add_subdirectory(extern/SDL)
add_subdirectory(extern/SDL_mixer)

include_directories(
    ${CMAKE_SOURCE_DIR}/extern
    ${CMAKE_SOURCE_DIR}/extern/AL
    ${CMAKE_SOURCE_DIR}/extern/glad
    ${CMAKE_SOURCE_DIR}/extern/glad/include
    ${CMAKE_SOURCE_DIR}/extern/glad/include/glad
    ${CMAKE_SOURCE_DIR}/extern/glm
    ${CMAKE_SOURCE_DIR}/extern/gltf
    ${CMAKE_SOURCE_DIR}/extern/json
    ${CMAKE_SOURCE_DIR}/extern/STB
    ${SFML_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/extern/freetype
    ${CMAKE_SOURCE_DIR}/Engine/src
)

add_subdirectory(Engine)

if (BUILD_GAME)
    add_subdirectory(Game)
endif()

if (BUILD_EDITOR)
    #add_subdirectory(Editor)
endif()
