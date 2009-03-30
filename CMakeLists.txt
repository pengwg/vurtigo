cmake_minimum_required (VERSION 2.6)
project (Vurtigo)

# The version number. 0.1
set (rtViewer_VERSION_MAJOR 0)
set (rtViewer_VERSION_MINOR 1)

####################
# Determine the OS
####################
if (UNIX)
  message(STATUS "Linux Found!")
elseif (APPLE)
  message(STATUS "Mac Found!")
elseif (WIN32)
  message(STATUS "Windows Found!")
endif (UNIX)

####################
# Setup for Qt
####################
find_package(Qt4)

# Qt Core and Qt Gui are included by default.
set(QT_USE_QTNETWORK 1)

include(${QT_USE_FILE})



add_subdirectory(UI)


set (SRC_FILES
  rtMain.cpp
)

include_directories(UI ${CMAKE_CURRENT_BINARY_DIR}/UI)
add_executable(vurtigo ${SRC_FILES})
target_link_libraries(vurtigo rtUI)
