cmake_minimum_required (VERSION 2.6)
project (Vurtigo)

# The version number. 0.1
set (rtViewer_VERSION_MAJOR 0)
set (rtViewer_VERSION_MINOR 1)

message(STATUS "Begin Install for Vurtigo: " ${rtViewer_VERSION_MAJOR} "." ${rtViewer_VERSION_MINOR})

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
find_package(Qt4 REQUIRED)

if (QT4_FOUND)
  # Qt Core and Qt Gui are included by default.
  set(QT_USE_QTNETWORK 1)

  include(${QT_USE_FILE})
endif(QT4_FOUND)

####################
# Setup for VTK
####################
find_package(VTK REQUIRED)
if (VTK_FOUND)
  message(STATUS "Found VTK: " ${VTK_MAJOR_VERSION} "." ${VTK_MINOR_VERSION} "." ${VTK_BUILD_VERSION})
  include(${USE_VTK_FILE})
endif(VTK_FOUND)


####################
# Start Building
####################

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include_directories(rtUI rtObject rtControl ${CMAKE_CURRENT_BINARY_DIR}/rtUI)

add_subdirectory(rtUI)
add_subdirectory(rtObject)
add_subdirectory(rtControl)

set (SRC_FILES
  rtMain.cpp
)

add_executable(vurtigo ${SRC_FILES})
target_link_libraries(vurtigo rtUI rtObject rtControl)
