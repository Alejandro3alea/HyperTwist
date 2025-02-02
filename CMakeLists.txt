cmake_minimum_required(VERSION 3.10)
project(HyperTwist VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_GAME "Build the game executable" ON)
option(BUILD_EDITOR "Build the editor executable" OFF)


include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/AL
    ${CMAKE_SOURCE_DIR}/include/GL
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/gltf
    ${CMAKE_SOURCE_DIR}/include/json
    ${CMAKE_SOURCE_DIR}/include/SDL2
    ${CMAKE_SOURCE_DIR}/include/STB
    ${CMAKE_SOURCE_DIR}/include/SFML
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