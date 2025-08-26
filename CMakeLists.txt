cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(XPlaneCommandAliasesPlugin CXX)

set(XPSDK_DIR "C:/XPSDK")         # Set the SDK path
set(XPLANE_DIR "C:/X-Plane 12")   # Set the X-Plane install path

add_library(win SHARED plugin.cpp)
set_target_properties(win PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(win PRIVATE
        "${XPSDK_DIR}/CHeaders/XPLM"
)

target_link_libraries(win PRIVATE
        "${XPSDK_DIR}/Libraries/Win/XPLM_64.lib"
        "${XPSDK_DIR}/Libraries/Win/XPWidgets_64.lib"
)

set_target_properties(win PROPERTIES
        OUTPUT_NAME "win"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/staging/win_x64"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/staging/win_x64"
        PREFIX ""
        SUFFIX ".xpl"
)

set(PLUGIN_DIR "${XPLANE_DIR}/Resources/plugins/NVANCommandAliases")

install(TARGETS win
        RUNTIME DESTINATION "${PLUGIN_DIR}/64"
        LIBRARY DESTINATION "${PLUGIN_DIR}/64"
)

set(COMMANDS_FILE "${PLUGIN_DIR}/commands.txt")
install(CODE "
    if(NOT EXISTS \"${COMMANDS_FILE}\")
        file(WRITE \"${COMMANDS_FILE}\" \"\")
    endif()
")
