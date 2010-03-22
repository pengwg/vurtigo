/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#ifndef RT_EP_DATA_OBJECT_H
#define RT_EP_DATA_OBJECT_H

#include "rtDataObject.h"

#include <QMultiMap>
#include <QMap>
#include <QList>
#include <QTimer>

#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkKochanekSpline.h>
#include <vtkTransform.h>
#include <vtkPoints.h>

//! Object that represents electro phisiological data
/*!
  EP Data object.
  */
class rtEPDataObject : public rtDataObject
{
Q_OBJECT

public:

  //! A single EP mesh point in 3D space
  struct EPPoint {
    unsigned int loc;
    double x, y, z;
  };

  //! All of the data for a single phase.
  struct PhaseData{
    int triggerDelay;
    //! Point list sorted and mapped by slice.
    QMultiMap <int, EPPoint> pointList;
    //! The vtk Point List. Not sorted in any wawy.
    vtkPoints* pointSet;
    //! Poly data representing the points.
    vtkPolyData* pointData;
    bool pointDataUpdate;

    //! Splines by slice.
    QMap <int, vtkKochanekSpline*> sliceSplineX;
    QMap <int, vtkKochanekSpline*> sliceSplineY;
    QMap <int, vtkKochanekSpline*> sliceSplineZ;

    //! Splines by position.
    QMap <int, vtkKochanekSpline*> posSplineX;
    QMap <int, vtkKochanekSpline*> posSplineY;
    QMap <int, vtkKochanekSpline*> posSplineZ;

    //! Poy data to show the mesh.
    vtkPolyData* meshData;
    bool meshDataUpdate;
  };

  //! Constructor
  rtEPDataObject();

  //! Destructor
  ~rtEPDataObject();

  void update();

  //! Set the object orientation and position through a transform.
  bool setTransform(vtkTransform* t);

  //! Set the trigger delay for a particular phase.
  bool setTriggerDelay(int phase, int trigger);

  //! Add a point to the mesh.
  bool addPoint(int phase, int slice, EPPoint pt);

  //! Get the list of points for a particular phase and slice.
  QList<EPPoint> getPoints(int phase, int slice);

  //! Get the point data for the current phase
  /*!
    Get the vtkPolyData object that can be rendered to represent the points in the mesh.
    The points are returned only for the current phase.
    \return A pointer to the poly data that can be used for rendering. NULL is returned on error.
    \sa setCurrPhase
    \sa getCurrPhase
    */
  vtkPolyData* getPointData();

  //! Get the mesh data for the current phase
  vtkPolyData* getMeshData();

  //! Set the trigger delay.
  /*!
    The phase will be calculated from the trigger delay and the current phase will be stored.
    \param trigDelay The trigger delay to set.
    \return True if the delay found the correct phase.
    */
  bool setCurrTrigDelay(int trigDelay);

  //! Set the value for the current phase.
  /*!
    The function fails if the phase value that is set does not have data associated with it.
    \param phase The phase value to set.
    */
  bool setCurrPhase(int phase);
  
  //! Get the value for the current phase.
  int getCurrPhase() { return m_currentPhase; }

  //! Check if a certain phase exists.
  bool phaseExists(int phaseNum);

  //! Create a phase that does not exist.
  void createPhase(int phaseNum);

  //! Delete a phase that exists.
  void deletePhase(int phaseNum);

  virtual bool saveFile(QFile *file);
  virtual bool loadFile(QFile *file);
 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  //! Update the renderable point object
  void updatePointData();
  //! Update the renderable mesh data
  void updateMeshData();

  //! The current phase
  int m_currentPhase;

  //! Maps phase number to phase data.
  QMap<int, PhaseData> m_phaseDataList;

  //! Transform for the mesh and points.
  vtkTransform* m_objTransform;
};

#endif 
