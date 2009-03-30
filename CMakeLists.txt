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

####################
# Setup for VTK
####################
find_package(VTK)
if (VTK_FOUND)
  include(${USE_VTK_FILE})
endif(VTK_FOUND)


####################
# Start Building
####################

add_subdirectory(UI)
add_subdirectory(rtObject)

set (SRC_FILES
  rtMain.cpp
)

include_directories(UI ${CMAKE_CURRENT_BINARY_DIR}/UI)
include_directories(rtObject ${CMAKE_CURRENT_BINARY_DIR}/rtObject)

add_executable(vurtigo ${SRC_FILES})
target_link_libraries(vurtigo rtUI)
