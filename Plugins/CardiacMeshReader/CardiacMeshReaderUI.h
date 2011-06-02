#ifndef CARDIAC_MESH_READERUI_H
#define CARDIAC_MESH_READERUI_H

#include <QWidget>
#include <QString>
#include <ui_CardiacMeshReader.h>

#include "DICOMFileReader.h"
#include "MeshFileReader.h"
#include "MeshPointSet.h"

class rtPolyDataObject;
class rtEPDataObject;

class CardiacMeshReaderUI : public QWidget, public Ui::CardiacMeshReader
{
  Q_OBJECT

public:
  CardiacMeshReaderUI();
  ~CardiacMeshReaderUI();

public slots:
  void newDirectory();
  void dirChooser();
  void page1Next();
  void page1Finish();

  void newDirectoryMesh();
  void dirChooserMesh();
  void minSliderChanged(int);
  void maxSliderChanged(int);
  void page2Next();
  void page2Back();
  void page2Finish();

  void namesChanged();
  void page3Back();
  void page3Finish();

protected:
  void connectSignals();

  bool loadPolyDataFromPoints(rtPolyDataObject* data, MeshPointSet::PointType type, double smoothStep=1.0);
  bool loadEPMeshFromPoints(rtEPDataObject* data, MeshPointSet::PointType type);

  QString m_lastDir;
  QString m_lastMeshDir;

  DICOMFileReader m_customReader;
  MeshFileReader m_meshReader;

  //! The ID of the newest volume
  int m_vol;

  //! The ID of the non smooth icontour
  int m_icontourNoSmooth;

  //! The ID of the non smooth ocontour
  int m_ocontourNoSmooth;

  //! The ID of the EP mesh for the icontour
  int m_icontourEPMesh;

  //! The ID of the EP mesh for the ocontour
  int m_ocontourEPMesh;
};

#endif
