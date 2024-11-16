cmake_minimum_required(VERSION 3.29)

project(ELS_MacOS_Launcher)
set(CMAKE_CXX_STANDARD 17)

set(MACOSX_BUNDLE_ICON_FILE els-logo-98px.icns)
set(ELS_NAVIGATOR_ICON ${CMAKE_CURRENT_SOURCE_DIR}/els-logo-98px.icns)
set_source_files_properties(${ELS_NAVIGATOR_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

add_executable(ELS-Navigator MACOSX_BUNDLE ${ELS_NAVIGATOR_ICON} main.cpp)

set_target_properties(ELS-Navigator PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "ELS-Navigator"
        MACOSX_BUNDLE_BUNDLE_VERSION "4.0.0"
        MACOSX_BUNDLE_COPYRIGHT "https://github.com/Corionis/ELS-MacOSD-Launcher, GPL-3.0 License"
        MACOSX_BUNDLE_GUI_IDENTIFIER "ELS-Navigator"
        MACOSX_BUNDLE_ICON_FILE els-logo-98px.icns
        MACOSX_BUNDLE_INFO_STRING "By Corionis"
        MACOSX_BUNDLE_LONG_VERSION_STRING "Version 4.0.0, 15 November 2024"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "Version 4.0.0, 15 November 2024"
)
