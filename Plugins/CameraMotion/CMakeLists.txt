
# List of source files.
set(CAMERA_MOTION_SRC_FILES
  CameraMotionPlugin.cpp
)

# MOC files are the ones the use Q_OBJECT
set (CAMERA_MOTION_MOC_FILES
  CameraMotionPlugin.h
)

qt4_wrap_cpp(CAMERA_MOTION_MOC_FILES_OUT ${CAMERA_MOTION_MOC_FILES} )
add_library(CameraMotion SHARED ${CAMERA_MOTION_SRC_FILES} ${CAMERA_MOTION_MOC_FILES_OUT})
target_link_libraries(CameraMotion ${QT_LIBRARIES} VurtigoBase)
