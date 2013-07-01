#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

project(kobby)
cmake_minimum_required(VERSION 2.6)
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules )

find_package(KDE4 REQUIRED)
find_package(Libqinfinity REQUIRED)

# TODO: make optional?
set(IS_KTP_INTERNAL_MODULE TRUE) # aw yeah
find_package(TelepathyQt4 0.8.9 REQUIRED)
find_package(KTp REQUIRED)

include(KDE4Defaults)

# find_package(Msgfmt REQUIRED)
# find_package(Gettext REQUIRED)
# add_subdirectory( po )

add_subdirectory(kte-plugin)
add_subdirectory(kioslave)
add_subdirectory(common)
add_subdirectory(tests)
