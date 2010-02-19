cmake_minimum_required (VERSION 2.6)
project (Vurtigo)

# The version number.
set (Vurtigo_VERSION_MAJOR 1)
set (Vurtigo_VERSION_MINOR 0)
set (Vurtigo_VERSION_REVISION 0)

set ( Vurtigo_PROJECT_VERSION "${Vurtigo_VERSION_MAJOR}.${Vurtigo_VERSION_MINOR}.${Vurtigo_VERSION_REVISION}"  )

message(STATUS "Begin Install for Vurtigo: " ${Vurtigo_PROJECT_VERSION} )

set(DEBUG_VERBOSE_MODE OFF CACHE BOOL "Build in Debug Verbose Mode. Slower but provides useful info.")

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
  set(QT_USE_QTXML 1)
  set(QT_USE_QTHELP 1)
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
# Doxygen target called 'doc'
####################

include(FindDoxygen)
if (DOXYGEN_DOT_PATH)
  set(DOXYGEN_DOT_FOUND YES)
endif(DOXYGEN_DOT_PATH)
include("TargetDoc.cmake" OPTIONAL)


####################
# Start Building
####################


message("Build Type: " ${CMAKE_BUILD_TYPE})

message("Debug Build Flags: " ${CMAKE_C_FLAGS_DEBUG})
message("Release Build Flags: " ${CMAKE_C_FLAGS_RELEASE})

set(QT_DEFINITIONS ${QT_DEFINITIONS} " " -DQT_NO_DEBUG_OUTPUT)
message("Qt Flags: " ${QT_DEFINITIONS})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

file(WRITE ${CMAKE_BINARY_DIR}/buildParam.h "#define VURTIGO_MAJOR_VERSION " ${Vurtigo_VERSION_MAJOR} "\n" )
file(APPEND ${CMAKE_BINARY_DIR}/buildParam.h "#define VURTIGO_MINOR_VERSION " ${Vurtigo_VERSION_MINOR} "\n" )
file(APPEND ${CMAKE_BINARY_DIR}/buildParam.h "#define VURTIGO_REVISION_VERSION " ${Vurtigo_VERSION_REVISION} "\n" )
file(APPEND ${CMAKE_BINARY_DIR}/buildParam.h "#define DEBUG_VERBOSE_MODE_" ${DEBUG_VERBOSE_MODE} "\n" )

include_directories(include ${CMAKE_BINARY_DIR})

# Include the DCMTK Vurtigo bridge code. 
include_directories(
  external/dcmtk-Vurtigo/ 
  external/dcmtk-Vurtigo/dcmtk/config
  external/dcmtk-Vurtigo/dcmtk/dcmdata
  external/dcmtk-Vurtigo/dcmtk/ofstd
  external/dcmtk-Vurtigo/Vurtigo
)


set (UI_FILES 
  rtUI/rtMainWindow.ui
  rtUI/propertyChooserDialog.ui
  rtUI/textPropertyDialog.ui
  rtUI/axesProperties.ui
  rtUI/volume3DOptions.ui
  rtUI/options2DView.ui
  rtUI/rtAboutDialog.ui
  rtUI/cathOptions.ui
  rtUI/newObjectDialog.ui
  rtUI/VtkRangeDialog.ui
  rtUI/VtkClampingWidget.ui
  rtUI/VtkRangeWidget.ui
  rtUI/slice2DOptions.ui
  rtUI/rtRenderOptions.ui
  rtUI/VtkColorTable.ui
  rtUI/rtWindowLevelDialog.ui
  rtUI/polyDataOptions.ui
  rtUI/rtTimeOptionsDialog.ui
)


qt4_wrap_ui(UI_FILES_OUT ${UI_FILES})
message(STATUS "UI Files: " ${UI_FILES_OUT})
add_custom_target(UIFirst DEPENDS ${UI_FILES_OUT})

add_subdirectory(external)
add_subdirectory(rtUI)
add_subdirectory(rtObject)
add_subdirectory(rtControl)
add_subdirectory(rtPlugin)

qt4_wrap_cpp(MOC_FILES_OUT ${RTOBJECT_MOC_FILES} ${RTCONTROL_MOC_FILES} ${RTUI_MOC_FILES} ${RTPLUGIN_MOC_FILES})

#message(STATUS "Moc Files: " ${MOC_FILES})

set (MAIN_SRC_FILES
  rtMain.cpp
)

add_library(VurtigoBase SHARED ${RTOBJECT_SRC_FILES} ${RTCONTROL_SRC_FILES} ${RTUI_SRC_FILES} ${RTPLUGIN_SRC_FILES} ${MOC_FILES_OUT})
target_link_libraries(VurtigoBase
  ${QT_LIBRARIES}
  vtkFiltering
  vtkCommon 
  vtkRendering 
  vtkVolumeRendering
  QVTK
)

add_executable(vurtigo ${MAIN_SRC_FILES})
target_link_libraries(vurtigo VurtigoBase)

add_dependencies(VurtigoBase UIFirst)

# After building the base we can now add the plugins.
add_subdirectory(Plugins)
