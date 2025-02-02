file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(Engine STATIC ${ALL_SOURCES})

target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
