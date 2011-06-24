/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef OBJ_TYPES_H
#define OBJ_TYPES_H

#include <QString>

//! A vurtigo container class for constants such as object types.
class rtConstants {

  public:
  //! An enumeration of the different types of objects
  enum rtObjectType {
    OT_None,
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
    OT_TextLabel,
    OT_EPMesh,
    OT_2DPlot
  };

  //! Convert the object type into an int.
  /*!
    \param type The object type as an enum.
    \return The same value but as an integer.
    */
  static int objectTypeToInt(rtObjectType type) {
    return (int)type;
  }

  //! Convert the object type into a QString.
  /*!
    \param type The object type as an enum.
    \return The same value but as an integer.
    */
  static QString objectTypeToQString(rtObjectType type)
  {
      QString result;
      switch (type) {
        case OT_None:
          result = "OT_None";
          break;
        case OT_3DObject:
          result = "OT_3DObject";
          break;
        case OT_2DObject:
          result = "OT_2DObject";
          break;
        case OT_Cath:
          result = "OT_Cath";
          break;
        case OT_vtkMatrix4x4:
          result = "OT_vtkMatrix4x4";
          break;
        case OT_vtkPolyData:
          result = "OT_vtkPolyData";
          break;
        case OT_vtkPiecewiseFunction:
          result = "OT_vtkPiecewiseFunction";
          break;
        case OT_vtkColorTransferFunction:
          result = "OT_vtkColorTransferFunction";
          break;
        case OT_ImageBuffer:
          result = "OT_ImageBuffer";
          break;
        case OT_2DPointBuffer:
          result = "OT_2DPointBuffer";
          break;
        case OT_3DPointBuffer:
          result = "OT_3DPointBuffer";
          break;
        case OT_TextLabel:
          result = "OT_TextLabel";
          break;
        default:
          result = "OT_None";
          break;
      }
      return result;
  }

  //! Convert an int into a variable of type rtObjectType.
  /*!
    \param type The object type as an integer.
    \return The same value but cast as an object type. Will return OT_None if the integer value is not valid.
    */
  static rtObjectType intToObjectType(int type) {
    rtObjectType result;
    switch (type) {
      case OT_None:
      case OT_3DObject:
      case OT_2DObject:
      case OT_Cath:
      case OT_vtkMatrix4x4:
      case OT_vtkPolyData:
      case OT_vtkPiecewiseFunction:
      case OT_vtkColorTransferFunction:
      case OT_ImageBuffer:
      case OT_2DPointBuffer:
      case OT_3DPointBuffer:
      case OT_TextLabel:
      case OT_EPMesh:
      case OT_2DPlot:
      result = (rtObjectType)type;
      break;
      default:
      result = OT_None;
      break;
    }
    return result;
  }

};

#endif
