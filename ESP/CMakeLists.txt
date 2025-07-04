# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# Ensure submodules are present so required headers like
# settings_lib.h exist. This avoids confusing build errors when the
# repository is cloned without the `--recursive` flag.
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/components/ESP32-RevK/settings_lib.h")
    message(STATUS "Fetching git submodules")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E env python "${CMAKE_CURRENT_LIST_DIR}/../setup_submodules.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.."
    )
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Faikin)
