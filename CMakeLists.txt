cmake_minimum_required(VERSION 3.5)

project(Funder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MACOSX_BUNDLE ON)

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
        src/proxy_model.cc include/proxy_model.h)

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
    else ()
        add_executable(Funder
                ${PROJECT_SOURCES}
                )
    endif ()
endif ()

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
