
set(SRC_FILES
    HumanSensorPlugin.cpp
    HumanSensor.cpp
)

set(MOC_FILES
    HumanSensorPlugin.h
    HumanSensor.h
)

include_directories (
    /usr/include/ni
    /usr/include/nite
)

qt4_wrap_cpp(MOC_FILES_OUT ${MOC_FILES})

add_library(HumanSensor SHARED ${SRC_FILES} ${MOC_FILES_OUT})
target_link_libraries(HumanSensor ${QT_LIBRARIES} VurtigoBase OpenNI XnVNite_1_5_2)
