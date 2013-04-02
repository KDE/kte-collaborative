#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

project(kobby)
cmake_minimum_required(VERSION 2.6)
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules )

find_package(KDE4 REQUIRED)
find_package(Libqinfinity REQUIRED)

include(KDE4Defaults)

# find_package(Msgfmt REQUIRED)
# find_package(Gettext REQUIRED)
# add_subdirectory( po )

add_subdirectory(kobby)
add_subdirectory(kioslave)
add_subdirectory(common)
