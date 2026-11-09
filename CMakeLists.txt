# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/components/spiffs/project_include.cmake)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(dashboard_alarm)

spiffs_create_partition_image(
    spiffs
    ${CMAKE_SOURCE_DIR}/spiffs
    FLASH_IN_PROJECT
)
