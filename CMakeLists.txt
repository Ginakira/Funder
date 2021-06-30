cmake_minimum_required(VERSION 3.5)

project(Funder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network Sql REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        src/mainwindow.cc
        include/mainwindow.h
        src/ui/mainwindow.ui
        include/networker.h
        src/networker.cc
        include/utility.h
        src/utility.cc
        include/database.h
        src/database.cc
        src/ui/holding_tab.ui
        include/holding_tab.h
        src/holding_tab.cc
        include/new_holding_dialog.h
        src/ui/new_holding_dialog.ui
        src/new_holding_dialog.cc
        include/fund_info.h
        src/fund_info.cc
        src/proxy_model.cc
        include/proxy_model.h
        src/my_sql_table_model.cc
        include/my_sql_table_model.h
        src/settings.cc
        include/settings.h
        include/buy_dialog.h
        src/buy_dialog.cc
        src/ui/buy_dialog.ui
        )

# 应用图标相关
# NOTE: Don't include the path in MACOSX_BUNDLE_ICON_FILE -- this is
# the property added to Info.plist
set(MACOSX_BUNDLE_ICON_FILE app_icon.icns)

# And this part tells CMake where to find and install the file itself
set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.icns)
set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app_win32.rc")


if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Funder
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
else ()
    if (ANDROID)
        add_library(Funder SHARED
                ${PROJECT_SOURCES}
                )
    elseif (APPLE)
        add_executable(Funder MACOSX_BUNDLE ${PROJECT_SOURCES} ${APP_ICON_MACOSX})
    elseif (WIN32)
        add_executable(Funder ${PROJECT_SOURCES} ${APP_ICON_RESOURCE_WINDOWS})
    else ()
        add_executable(Funder ${PROJECT_SOURCES})
    endif ()
endif ()

# Windows下编译隐藏console
if (WIN32)
    if (MSVC)
        set_target_properties(Funder PROPERTIES
                WIN32_EXECUTABLE YES
                LINK_FLAGS "/ENTRY:mainCRTStartup"
                )
    elseif (CMAKE_COMPILER_IS_GNUCXX)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
    else ()
        message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
    endif (MSVC)
elseif (APPLE)
    set_target_properties(Funder PROPERTIES
            MACOSX_BUNDLE YES
            )
elseif (UNIX)
    # Nothing special required
else ()
    message(SEND_ERROR "You are on an unsupported platform! (Not Win32, Mac OS X or Unix)")
endif (WIN32)


target_link_libraries(Funder PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Sql
        )

set_target_properties(Funder PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER pmrfansub.tk
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Funder)
endif ()
