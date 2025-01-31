cmake_minimum_required(VERSION 3.10)
project(HyperTwist VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_GAME "Build the game executable" ON)
option(BUILD_EDITOR "Build the editor executable" ON)

add_subdirectory(Engine)

if (BUILD_GAME)
    add_subdirectory(Game)
endif()

if (BUILD_EDITOR)
    add_subdirectory(Editor)
endif()