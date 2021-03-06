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
#ifndef RT_EP_DATA_OBJECT_H
#define RT_EP_DATA_OBJECT_H

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

#include "rtDataObject.h"
#include "rtEPInfoObject.h"
#include "ui_EPOptions.h"
#include "cineWidget.h"
#include "rtCardiacMeshPointData.h"
#include "ObjectSelectionComboBox.h"

//! Object that represents electro phisiological data
/*!
  EP Data object.
  */
class rtEPDataObject : public rtDataObject
{
Q_OBJECT

public:

  //! All of the data for a single phase.
  struct PhaseData{
    int triggerDelay;
    //! Point list sorted and mapped by slice.
    QMultiMap <int, rtCardiacMeshPointData> pointList;
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

  //! Get the object orientation and position through a transform.
  inline vtkTransform* getTransform() { return m_objTransform; }

  //! Set the trigger delay for a particular phase.
  bool setTriggerDelay(int phase, int trigger);

  //! Add a point to the mesh.
  bool addPoint(rtCardiacMeshPointData pt);

  //! Get the list of points for a particular phase and slice.
  QList<rtCardiacMeshPointData> getPoints(int phase, int slice);

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

  //! Get the color function for this mesh.
  vtkColorTransferFunction* getColorFunction() { return m_EPInfoObject.getColorFunction(); }

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

  //! Add a point with related EP info
  void addInfoPoint(rtNamedInfoPointData point);

  //! Check if the info points should be shown
  bool showInfoPoints() { return m_showInfoPoints; }

  //! The the poly data for the info points.
  vtkPolyData* getInfoPolyData() { return m_EPInfoObject.getPointPolyData(); }

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

  void inSliceValueChanged(double);
  void betweenSliceValueChanged(double);

  void colorByPropertyChanged(int);

  void effectSizeChanged(int);

  void showPointsChanged(bool);

  //! The object that supplies the carto point information has changed.
  void cartoPointObjectChanged(int);

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

  //! used for loading from file
  bool readNewPhaseFromFile(QXmlStreamReader* reader, int phase, int trigger);

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
  ObjectSelectionComboBox m_objectSelectionBox;

  //! The user defined opacity for the surface
  double m_surfaceOpacity;

  //! The user defined opacity for the points
  double m_pointsOpacity;

  //! The representation for the mesh
  EPSurfaceRep m_rep;

  //! Number of threads that can be used.
  int m_threadCount;

  //! Minimum slice number
  unsigned int m_minSliceNum;
  //! Maximum slice number
  unsigned int m_maxSliceNum;

  //! Object to hold all of the points that have EP info.
  rtEPInfoObject m_EPInfoObject;

  //! Boolean to determine if the EP info points are shown
  bool m_showInfoPoints;

  //! Task that can create the position splines
  class CreatePositionSplinesTask : public QRunnable {
    public:
      CreatePositionSplinesTask(PhaseData* phase, int coord, double maxPosition, double planeInterval, QList<int>* slices, int minSlice, int maxSlice)
      : m_phase(phase), m_slices(slices), m_coord(coord), m_maxPosition(maxPosition), m_planeInterval(planeInterval), m_minSlice(minSlice), m_maxSlice(maxSlice)
      { }

      void run() {
        vtkKochanekSpline *tempSpline;
        double pos = 0.0;

        for (pos=0.0; pos<=m_maxPosition;) {
          tempSpline = vtkKochanekSpline::New();
          m_phase->posSpline[m_coord].insert(pos, tempSpline);

          for (int ix1=0; ix1<m_slices->size(); ix1++) {
            if (m_slices->at(ix1) < m_minSlice || m_slices->at(ix1) > m_maxSlice) continue;
            tempSpline->AddPoint( ix1, m_phase->sliceSpline[m_coord].value(m_slices->at(ix1))->Evaluate(pos) );
          }

          if (pos < m_maxPosition && (pos+m_planeInterval) > m_maxPosition) {
            pos = m_maxPosition;
          } else {
            pos+=m_planeInterval;
          }
          tempSpline->Compute();
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
