set(TARGET_NAME Engine)

file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(${TARGET_NAME} STATIC ${ALL_SOURCES} "src/Drivers/Audio/WDMKS/WDMKS_AudioDriver.cpp" "src/Drivers/Audio/AudioDriver.h" "src/Drivers/Audio/AudioDriver.cpp" "src/Misc/Concepts.h" "src/Drivers/DriverMgr.h" "src/Drivers/Audio/WDMKS/WDMKS_Pin.h" "src/Drivers/Audio/WDMKS/WDMKS_Pin.cpp" "src/Drivers/Audio/WDMKS/WDMKS_Filter.h" "src/Drivers/Audio/WDMKS/WDMKS_Filter.cpp" "src/Drivers/Audio/WDMKS/WDMKS_Stream.h" "src/Drivers/Audio/WDMKS/WDMKS_Stream.cpp" "src/Drivers/Audio/WDMKS/WDMKS_Exception.h" "src/Drivers/Audio/WDMKS/WDMKS_Common.h" "src/Drivers/Audio/WDMKS/WDMKS_Common.cpp" "src/Audio/AudioMixBuffer.h" "src/Audio/AudioMixBuffer.cpp")

# Linux config
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Setting up libraries for Linux in ${TARGET_NAME} target...")
    target_link_directories(${TARGET_NAME} PRIVATE /usr/lib)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            GL
            GLEW
            freetype
    )

else() # Windows
    message("Setting up libraries for Windows in ${TARGET_NAME} target...")

endif()


target_compile_definitions(${TARGET_NAME} PUBLIC SFML_STATIC)

target_compile_definitions(${TARGET_NAME} PRIVATE ENGINE_EXPORTS)
