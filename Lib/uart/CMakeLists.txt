cmake_minimum_required(VERSION 3.22)

project(UtilsLib LANGUAGES C CXX)

add_library(UtilsLib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/uartTx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uartRx.cpp
)

target_include_directories(UtilsLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(UtilsLib PUBLIC
    stm32cubemx
)

set_target_properties(UtilsLib PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)
