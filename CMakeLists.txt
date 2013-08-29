#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

project(kobby)
cmake_minimum_required(VERSION 2.6)
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules )

set(KTECOLLAB_VERSION_MAJOR 0)
set(KTECOLLAB_VERSION_MINOR 0)
set(KTECOLLAB_VERSION_PATCH 95)
set(KTECOLLAB_VERSION_STRING ${KTECOLLAB_VERSION_MAJOR}.${KTECOLLAB_VERSION_MINOR}.${KTECOLLAB_VERSION_PATCH})

configure_file(version.h.in version.h)

find_package(KDE4 REQUIRED)
find_package(Libqinfinity REQUIRED)

# TODO: make optional?
set(IS_KTP_INTERNAL_MODULE TRUE) # aw yeah
find_package(TelepathyQt4 0.8.9 REQUIRED)
find_package(KTp REQUIRED)

include(KDE4Defaults)

add_definitions("-DENABLE_TAB_HACK")

add_subdirectory(kte-plugin)
add_subdirectory(kioslave)
add_subdirectory(common)
add_subdirectory(tests)
add_subdirectory(kded-module)
