cmake_minimum_required(VERSION 3.5)
project(l_sys_tree_generator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/qt_ui/)

file(GLOB_RECURSE SOURCES
        "${PROJECT_SOURCE_DIR}/*.cpp"
        "${PROJECT_SOURCE_DIR}/*.hpp"
        "${PROJECT_SOURCE_DIR}/*.h"
        "${PROJECT_SOURCE_DIR}/*.ui"
        "${PROJECT_SOURCE_DIR}/*.qrc"
)

list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_BINARY_DIR}")
list(FILTER SOURCES EXCLUDE REGEX "CMakeFiles")
list(FILTER SOURCES EXCLUDE REGEX "cmake-build-")
list(FILTER SOURCES EXCLUDE REGEX "_autogen")

set(PROJECT_ROOT_DIR .)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(l_sys_tree_generator ${SOURCES})



target_include_directories(l_sys_tree_generator PRIVATE
        ${PROJECT_ROOT_DIR}
        ${PROJECT_ROOT_DIR}/objects
        ${PROJECT_ROOT_DIR}/objects/mesh
        ${PROJECT_ROOT_DIR}/camera
        ${PROJECT_ROOT_DIR}/camera/free_camera
        ${PROJECT_ROOT_DIR}/camera/orbit_camera
        ${PROJECT_ROOT_DIR}/camera/camera_factory
        ${PROJECT_ROOT_DIR}/managers
        ${PROJECT_ROOT_DIR}/managers/camera_manager
        ${PROJECT_ROOT_DIR}/scene
        ${PROJECT_ROOT_DIR}/geometry
        ${PROJECT_ROOT_DIR}/lsystem
        ${PROJECT_ROOT_DIR}/renderer
        ${PROJECT_ROOT_DIR}/renderer/rasterizer
        ${PROJECT_ROOT_DIR}/renderer/texture_loader
        ${PROJECT_ROOT_DIR}/renderer/z-buffer
        ${PROJECT_ROOT_DIR}/renderer/lighting
        ${PROJECT_ROOT_DIR}/renderer/shadow-mapping
        ${PROJECT_ROOT_DIR}/visitor
        ${PROJECT_ROOT_DIR}/ui
)

target_link_libraries(l_sys_tree_generator
        Qt::Core
        Qt::Gui
        Qt::Widgets
)