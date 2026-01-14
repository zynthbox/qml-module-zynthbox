# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

option(BUILD_WITH_QT5 "Use Qt 5" OFF)
option(BUILD_WITH_QT6 "Use Qt 6" OFF)

set(QT_MAJOR_VERSION 5)
set(KF_MAJOR_VERSION 5)

set(REQUIRED_QT_VERSION 5.15.2)
set(REQUIRED_KF_VERSION 5.95.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(ZB_URI io.zynthbox.ui)

set(AUTOMOC_MOC_OPTIONS -Muri=${ZB_URI})

project(zynthbox-qml-module VERSION 1.0)

find_package(ECM ${REQUIRED_KF_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

#DEFAULT COMPONENTS DEFINITIONS
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMGenerateQmlTypes)
include(ECMSourceVersionControl)

include(KDEClangFormat)
include(KDEInstallDirs)
include(KDECMakeSettings)

option(QUICK_COMPILER "Use QtQuick compiler to improve performance. QML sources won't be installed in the file system." ON)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Core
    Qml
    Quick
    QuickControls2
    Svg
    Widgets)

find_package(KF${KF_MAJOR_VERSION} ${REQUIRED_KF_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Plasma
    Config)
 #   find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Plasma)
 # find_package(Plasma REQUIRED)
if(QUICK_COMPILER)
    find_package(Qt5QuickCompiler)
    set_package_properties(Qt5QuickCompiler PROPERTIES
        DESCRIPTION "Compile QML at build time"
        TYPE OPTIONAL)
endif()

add_compile_definitions(QT_DISABLE_DEPRECATED_UP_TO=0x050F00)

add_subdirectory(controls)

##Clang formatting
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
