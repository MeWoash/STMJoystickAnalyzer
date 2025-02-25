cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME tft_display)
set(GIT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(CXX ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)
add_subdirectory(Lib/uart)
add_subdirectory(Lib/lvgl)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/alt_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/lcd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/lvgl_adapter.cpp
)

file(GLOB UI_C_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/ui/*.c)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${UI_C_FILES})

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/ui
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

target_compile_definitions(lvgl PUBLIC 
    LV_CONF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/Lib/lvgl/lv_conf.h
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    UtilsLib
    lvgl
    lvgl_examples
    # Add user defined libraries
)

