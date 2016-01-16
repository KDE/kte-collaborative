#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

project(kobby)
cmake_minimum_required(VERSION 2.6)
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules )

include(FeatureSummary)
find_package(ECM 5.17.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(KTECOLLAB_VERSION_MAJOR 0)
set(KTECOLLAB_VERSION_MINOR 5)
set(KTECOLLAB_VERSION_PATCH 80)
set(KTECOLLAB_VERSION_STRING ${KTECOLLAB_VERSION_MAJOR}.${KTECOLLAB_VERSION_MINOR}.${KTECOLLAB_VERSION_PATCH})
set(LIBINFINITY_VERSION 0.6) # TODO use this for finding libinfinity

configure_file(version.h.in version.h)

find_package(Qt5 5.4 REQUIRED Core Widgets Script PrintSupport Xml XmlPatterns)

find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Parts ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5IconThemes ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5TextEditor ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5CoreAddons ${KF5_DEP_VERSION} REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS} KF5::Parts KF5::TextEditor KF5::ConfigWidgets)
include_directories("/usr/include/KF5/KTextEditor")
include_directories("/usr/include/KF5/KParts")
include_directories("/usr/include/KF5/KXmlGui")
include_directories("/usr/include/KF5/KCoreAddons")
include_directories("/usr/include/KF5/KIOCore")
include_directories("/usr/include/KF5/KConfigCore")
include_directories("/usr/include/KF5/KI18n")
include_directories("/usr/include/KF5/KConfigWidgets")
include_directories("/usr/include/KF5/KWidgetsAddons")
include_directories("/usr/include/KF5/KCMUtils")
include_directories("/usr/include/KF5/KIOWidgets")
include_directories("/usr/include/KF5/KConfigGui")
include_directories("/usr/include/KF5/KService")

find_package(Libqinfinity REQUIRED)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

add_definitions("-DENABLE_TAB_HACK")

add_subdirectory(kte-plugin)
add_subdirectory(kioslave)
add_subdirectory(common)
add_subdirectory(tests)
add_subdirectory(notifier-module)
