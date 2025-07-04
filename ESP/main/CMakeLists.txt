set(REVK_DIR ${CMAKE_CURRENT_LIST_DIR}/../components/ESP32-RevK)
set(SETTINGS_DEF ${CMAKE_CURRENT_LIST_DIR}/settings.def)
set(REVK_SETTINGS ${REVK_DIR}/revk_settings)

# Skip generation step when this file is parsed by the
# component_get_requirements.cmake script. That script sets
# BUILD_PROPERTIES_FILE, so use its absence to detect a real build.
if(NOT DEFINED BUILD_PROPERTIES_FILE)
    add_custom_command(
        OUTPUT settings.c settings.h
        COMMAND ${REVK_SETTINGS} ${SETTINGS_DEF} -h settings.h -c settings.c
        DEPENDS ${SETTINGS_DEF} ${REVK_DIR}/settings.def ${REVK_SETTINGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    set_source_files_properties(settings.c PROPERTIES GENERATED TRUE)
endif()

idf_component_register(
    SRCS "cn_wired_driver.c" "Faikin.c" "bleenv.c" "settings.c"
    INCLUDE_DIRS "." "../include"
    REQUIRES ESP32-RevK
    EMBED_FILES "favicon.ico" "apple-touch-icon.png"
)
