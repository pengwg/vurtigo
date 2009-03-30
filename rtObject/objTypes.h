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

}

#endif
