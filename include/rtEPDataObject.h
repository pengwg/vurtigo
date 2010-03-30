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
#include "ui_EPOptions.h"
#include "cineWidget.h"

#include <QMultiMap>
#include <QMap>
#include <QList>
#include <QTimer>
#include <QRunnable>

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
    //! Poly data representing the points.
    vtkPolyData* pointData;
    vtkProperty* pointProperty;
    bool pointDataUpdate;

    //! Splines by slice. Ordered x, y, z
    QMap <int, vtkKochanekSpline*> sliceSpline[3];

    //! Splines by position. Ordered x, y, z
    QMap <double, vtkKochanekSpline*> posSpline[3];

    //! Poy data to show the mesh.
    vtkPolyData* meshData;
    vtkProperty* meshProperty;
    bool meshDataUpdate;
  };

  enum EPSurfaceRep{
    EP_SURFACE=0,
    EP_WIREFRAME,
    EP_POINTS
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

  //! Get the property for the current phase of points.
  vtkProperty* getPointProperty();

  //! Get the mesh data for the current phase
  vtkPolyData* getMeshData();

  //! Get the property for the current phase of points.
  vtkProperty* getMeshProperty();

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
 public slots:
  void surfaceOpacityChanged(int);
  void pointsOpacityChanged(int);
  void triggerChanged(int);

  void minSliceChanged(int);
  void maxSliceChanged(int);
  void updateObjectNow();

  void representationChanged(int);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();

  //! Properly remove all elements from the slice spline list
  void cleanupSliceSpline(PhaseData* data);
  //! Properly remove all elements from the position spline list.
  void cleanupPositionSpline(PhaseData* data);

  //! Update the renderable point object
  void updatePointData(int);
  //! Update the renderable mesh data
  void updateMeshData(int);

  //! Update the renderable point object
  void updatePointProperty();
  //! Update the renderable mesh data
  void updateMeshProperty();

  //! Set the flag for all the data pieces to modified.
  void setModifyFlagForAll();

  //! The current phase
  int m_currentPhase;

  //! Maps phase number to phase data.
  QMap<int, PhaseData> m_phaseDataList;

  //! Transform for the mesh and points.
  vtkTransform* m_objTransform;

  double m_inPlaneInterval, m_crossPlaneInterval;

  //! The widget for the options for this object
  Ui::EPOptions m_optionsWidget;

  CineWidget m_cineWidget;

  //! The user defined opacity for the surface
  double m_surfaceOpacity;

  //! The user defined opacity for the points
  double m_pointsOpacity;

  //! The representation for the mesh
  EPSurfaceRep m_rep;

  //! Number of threads that can be used.
  int m_threadCount;

  //! Minimum slice number
  int m_minSliceNum;
  //! Maximum slice number
  int m_maxSliceNum;

  //! Task that can create the position splines
  class CreatePositionSplinesTask : public QRunnable {
    public:
      CreatePositionSplinesTask(PhaseData* phase, int coord, double maxPosition, double planeInterval, QList<int>* slices, int minSlice, int maxSlice) {
        m_phase = phase;
        m_coord = coord;
        m_maxPosition = maxPosition;
        m_planeInterval = planeInterval;
        m_slices = slices;
        m_minSlice = minSlice;
        m_maxSlice = maxSlice;
      }

      void run() {
        vtkKochanekSpline *tempSpline[3];
        for (double pos=0; pos<=m_maxPosition; pos+=m_planeInterval) {
          tempSpline[m_coord] = vtkKochanekSpline::New();
          m_phase->posSpline[m_coord].insert(pos, tempSpline[m_coord]);

          for (int ix1=0; ix1<m_slices->size(); ix1++) {
            if (m_slices->at(ix1) < m_minSlice || m_slices->at(ix1) > m_maxSlice) continue;
            tempSpline[m_coord]->AddPoint( ix1, m_phase->sliceSpline[m_coord].value(m_slices->at(ix1))->Evaluate(pos) );
          }
        }
      }

    protected:
      PhaseData* m_phase;
      QList<int>* m_slices;
      int m_coord;
      double m_maxPosition, m_planeInterval;
      int m_minSlice, m_maxSlice;
  };



};

#endif 
