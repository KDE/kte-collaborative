#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

project(kobby)
cmake_minimum_required(VERSION 2.6)
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules )
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

SUBDIRS(kobby)
