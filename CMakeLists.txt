# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#append mi LINK & Kconfig ERROR SOLUTION !!!
list(APPEND EXTRA_COMPONENT_DIRS .pio/libdeps/esp32cam)
project(ESP32CAM-PIO-IDF-STREAM)
