cmake_minimum_required(VERSION 3.10)
project(HyperTwist VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_GAME "Build the game executable" OFF)
option(BUILD_EDITOR "Build the editor executable" OFF)

# Prefer GLVND over the legacy OpenGL libraries.
set(OpenGL_GL_PREFERENCE GLVND)

find_package(SFML 2.5 COMPONENTS system audio REQUIRED)
set(SFML_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/SFML")
set(SFML_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/SFML")


# Linux config
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories(/usr/include)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/AL
    ${CMAKE_SOURCE_DIR}/include/GL
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/gltf
    ${CMAKE_SOURCE_DIR}/include/json
    ${CMAKE_SOURCE_DIR}/include/SDL2
    ${CMAKE_SOURCE_DIR}/include/STB
    ${SFML_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include/freetype
    ${CMAKE_SOURCE_DIR}/Engine/src
)

add_subdirectory(Engine)

if (BUILD_GAME)
    add_subdirectory(Game)
endif()

if (BUILD_EDITOR)
    add_subdirectory(Editor)
endif()
