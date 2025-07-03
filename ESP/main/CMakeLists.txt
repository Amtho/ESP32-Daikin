set(REVK_DIR ${CMAKE_CURRENT_LIST_DIR}/../components/ESP32-RevK)
set(SETTINGS_DEF ${CMAKE_CURRENT_LIST_DIR}/settings.def)
set(REVK_SETTINGS ${REVK_DIR}/revk_settings)

add_custom_command(
    OUTPUT settings.c settings.h
    COMMAND ${REVK_SETTINGS} ${SETTINGS_DEF} -h settings.h -c settings.c
    DEPENDS ${SETTINGS_DEF} ${REVK_DIR}/settings.def ${REVK_SETTINGS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
set_source_files_properties(settings.c PROPERTIES GENERATED TRUE)

set(COMPONENT_SRCS "cn_wired_driver.c" "Faikin.c" "bleenv.c" "settings.c")
set(COMPONENT_REQUIRES "ESP32-RevK")
set(COMPONENT_EMBED_FILES "favicon.ico" "apple-touch-icon.png")
register_component()
