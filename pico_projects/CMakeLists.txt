# Minimum version of CMake required
cmake_minimum_required(VERSION 3.12)

# Set the project name and supported languages (C, C++, and ASM)
project(enable_quad_spi_project C CXX ASM)

# Include the Pico SDK
# Manually set the SDK path if the PICO_SDK_PATH variable is not recognized
set(PICO_SDK_PATH "C:/Users/wilpa/pico-sdk")  # Replace with your actual SDK path

# Include the Pico SDK
include(${PICO_SDK_PATH}/pico_sdk_import.cmake)

# Initialize the Pico SDK
pico_sdk_init()

# Add an executable target for your project
# Replace 'pico_enable_quad_spi.c' with the actual name of your source file if different
add_executable(enable_quad_spi pico_enable_quad_spi.c)

# Link the necessary libraries for the project
target_link_libraries(enable_quad_spi pico_stdlib hardware_spi)

# Enable USB for standard I/O and disable UART (optional)
pico_enable_stdio_usb(enable_quad_spi 1)
pico_enable_stdio_uart(enable_quad_spi 0)

# Create additional output formats such as UF2, BIN, and HEX files
pico_add_extra_outputs(enable_quad_spi)
