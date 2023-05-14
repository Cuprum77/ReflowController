# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Set the project name
project(MCP9600)

# Add the library with the above sources
add_library(${PROJECT_NAME} src/MCP9600.cpp)
add_library(sub::MCP9600 ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_i2c
)