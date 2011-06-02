#ifndef RT_POLY_DATA_OBJECT_H
#define RT_POLY_DATA_OBJECT_H

#include <QList>
#include <QTimer>

#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkColorTransferFunction.h>

#include "rtDataObject.h"
#include "ui_polyDataOptions.h"
#include "rtBasic3DPointData.h"

//! Object that represents polygonal data
/*!
  Polygon Data object. Objects are composed of a set of points and a set of lines to connect those points.
  */
class rtPolyDataObject : public rtDataObject
{
Q_OBJECT

public:

  //! A link between three poly data points.
  /*!
    To create polygonal data the points must be joined into polygons that will be rendered as surfaces.
To join three points create an object such as this one and give the number of each point. Numbering starts at ZERO.
Points should be joined into triangles to be rendered.
    */
  struct PolyPointLink {
    //! The three poly points that make up the polygon.
    int threeVertex[3];
  };

  rtPolyDataObject();
  ~rtPolyDataObject();

  void update();

  vtkPolyData* getPolyData();
  vtkProperty* getProperty();
  vtkColorTransferFunction* getColorTable();

  //! Clear all of the poly data.
  /*!
   Removes and deletes all of the poly data from all the internal lists. This should reset the object to the initial state. 
   */
  void clearAllData();
  
  //! Set the points and connections for a new poly dataset.
  /*!
  This function will remove the old poly dataset and replace it based on the input parameters.
  Long computations are performed in this function so resetting of the geometry should not be done all the time.
  @return True if the replacement was a success.
  */
  bool setNewGeometry(QList<rtBasic3DPointData> *pts, QList<PolyPointLink> *links, int trigDelay=0);
  bool copyPolyData(vtkPolyData*);
  bool copyLookupTable(vtkColorTransferFunction*);

  bool setCurrTrigDelay(int trigDelay);
  bool setCurrPhase(int phase);
  
  int getCurrPhase() { return m_currentPhase; }

  virtual bool saveFile(QFile *file);
  virtual bool loadFile(QFile *file);
public slots:
  void opacityChanged(int);
  void meshCheckBoxChanged(int);

  void setVisibleComponent(int);
  void nextVisibleComponent();
  void cineLoop(bool);

 protected:
  // Functions
  void setupGUI();
  void cleanupGUI();
  
  bool readNewPhaseFromFile(QXmlStreamReader* reader, int pahse, int trigger);

  //! The widget for the options for this object
  Ui::polyDataOptions m_optionsWidget;

  QList<int> m_trigDelayList;
  QList<vtkPolyData*> m_polyData;
  QList<vtkProperty*> m_polyProperty;
  QList<vtkColorTransferFunction*> m_colorLookup;
  int m_currentPhase;

  QTimer *m_cineFrame;

};

#endif 
