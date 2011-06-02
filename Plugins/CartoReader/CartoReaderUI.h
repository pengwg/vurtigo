#ifndef CARTO_READER_UI_H
#define CARTO_READER_UI_H

#include <QWidget>
#include <QString>
#include <ui_CartoReader.h>

#include "vtkPoints.h"

#include "CartoFileReader.h"
#include "rtPolyDataObject.h"

class CartoReaderUI : public QWidget, public Ui::CartoReader
{
  Q_OBJECT

public:
  CartoReaderUI();
  ~CartoReaderUI();

public slots:
  void loadFile();
  void loadXmlFile();
  void saveAsPoints();
  void saveAsSurface();
  void filterByTriggerDelay();

protected:
  //! Connect the signals and slots from the GUI.
  void connectSignals();

  //! Create the set of polygons from the points.
  bool runVtkDelaunay(vtkPoints* pointsIn, QList<rtPolyDataObject::PolyPointLink> &linkListOut);

  //! Set the info in the table based on the reader.
  void resetTableInfo();

  //! Select the color for this point based on the bi-polar voltage.
  void selectPointColor(double biPolar, double color[3]);

  //! Name of the loaded file.
  QString m_fName;

  //! The instance of the class that reads the carto file.
  CartoFileReader m_customReader;

  //! The ID of the points set object.
  int m_pts;

  //! The ids of the three types of surface objects
  int m_surf[3];

  //! The id of the currenly selected point.
  int m_currPointSelection;

};

#endif
