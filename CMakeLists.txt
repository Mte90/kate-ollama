cmake_minimum_required(VERSION 3.25)

project(kateollama VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(ECM ${KF5_DEP_VERSION} QUIET REQUIRED NO_MODULE)

list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)

set(QT_MAJOR_VERSION "6")
set(QT_MIN_VERSION "6.5.0")
set(KF6_DEP_VERSION "6.0.0")
find_package(KF6 ${KF6_DEP_VERSION}
    REQUIRED COMPONENTS
        CoreAddons # Core addons on top of QtCore
        I18n # For localization
        TextEditor # The editor component
)

set(PROJECT_SOURCES
    src/plugin.cpp
    src/plugin.h
    src/settings.h
    src/settings.cpp
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

kcoreaddons_add_plugin(kateollama INSTALL_NAMESPACE "kf6/ktexteditor")
target_compile_definitions(kateollama PRIVATE TRANSLATION_DOMAIN="kateollama")
target_link_libraries(kateollama PRIVATE KF6::I18n KF6::TextEditor)
target_sources(kateollama  PRIVATE ${PROJECT_SOURCES})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
