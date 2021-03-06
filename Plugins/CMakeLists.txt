
set(VURTIGO_PLUGIN_GEOM_SERVER ON CACHE BOOL "Compile the Geom Server Plugin")
if (VURTIGO_PLUGIN_GEOM_SERVER)
add_subdirectory(GeomServer)
endif(VURTIGO_PLUGIN_GEOM_SERVER)

set(VURTIGO_PLUGIN_HELLO_WORLD ON CACHE BOOL "Compile the Hello World Plugin")
if (VURTIGO_PLUGIN_HELLO_WORLD)
add_subdirectory(HelloWorld)
endif (VURTIGO_PLUGIN_HELLO_WORLD)

set(VURTIGO_PLUGIN_TEST_SUITE ON CACHE BOOL "Compile the Test Suite Plugin")
if (VURTIGO_PLUGIN_TEST_SUITE)
add_subdirectory(TestSuite)
endif (VURTIGO_PLUGIN_TEST_SUITE)

set(VURTIGO_PLUGIN_DICOM_READER ON CACHE BOOL "Compile the Image Reader Plugin")
if (VURTIGO_PLUGIN_DICOM_READER)
add_subdirectory(ImageReader)
endif (VURTIGO_PLUGIN_DICOM_READER)

set(VURTIGO_PLUGIN_CARDIACMESH_READER ON CACHE BOOL "Compile the Cardiac Mesh Reader Plugin")
if (VURTIGO_PLUGIN_CARDIACMESH_READER)
add_subdirectory(CardiacMeshReader)
endif (VURTIGO_PLUGIN_CARDIACMESH_READER)

set(VURTIGO_PLUGIN_SCREEN_CAPTURE ON CACHE BOOL "Compile the Screen Capture Plugin")
if (VURTIGO_PLUGIN_SCREEN_CAPTURE)
add_subdirectory(ScreenCapture)
endif (VURTIGO_PLUGIN_SCREEN_CAPTURE)

set(VURTIGO_PLUGIN_CATH_TRACKING ON CACHE BOOL "Compile the Catheter Tracking Plugin")
if (VURTIGO_PLUGIN_CATH_TRACKING)
add_subdirectory(CathTracking)
endif (VURTIGO_PLUGIN_CATH_TRACKING)

set(VURTIGO_PLUGIN_CATH_HISTORY ON CACHE BOOL "Compile the Catheter History Plugin")
if (VURTIGO_PLUGIN_CATH_HISTORY)
add_subdirectory(CathHistory)
endif (VURTIGO_PLUGIN_CATH_HISTORY)

set(VURTIGO_PLUGIN_ALIGNMENT_TOOL ON CACHE BOOL "Compile the Alignment Tool Plugin")
if (VURTIGO_PLUGIN_ALIGNMENT_TOOL)
add_subdirectory(AlignmentTool)
endif (VURTIGO_PLUGIN_ALIGNMENT_TOOL)

set(VURTIGO_PLUGIN_CARTO_READER ON CACHE BOOL "Compile the Carto Reader Plugin")
if (VURTIGO_PLUGIN_CARTO_READER)
add_subdirectory(CartoReader)
endif (VURTIGO_PLUGIN_CARTO_READER)

set(VURTIGO_PLUGIN_CAMERA_MOTION ON CACHE BOOL "Compile the Camera Motion Plugin")
if (VURTIGO_PLUGIN_CAMERA_MOTION)
add_subdirectory(CameraMotion)
endif (VURTIGO_PLUGIN_CAMERA_MOTION)
