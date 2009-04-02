#ifndef OBJ_TYPES_H
#define OBJ_TYPES_H

namespace rtConstants {

enum rtObjectType {
  OT_None,
  OT_4DObject,
  OT_3DObject,
  OT_2DObject,
  OT_Cath,
  OT_vtkMatrix4x4,
  OT_vtkPolyData,
  OT_vtkPiecewiseFunction,
  OT_vtkColorTransferFunction,
  OT_ImageBuffer,
  OT_2DPointBuffer,
  OT_3DPointBuffer,
  OT_TextLabel
};

/*! Threshold for read-only objects.
  Some data objects are created by the base and while the plugins may be able to read them they will not be able to modify the contents. All objects with ID less than this threshold cannot be modified by a plugin. 
*/
 const int ReadOnlyObjectThresh = 1000;

}

#endif
