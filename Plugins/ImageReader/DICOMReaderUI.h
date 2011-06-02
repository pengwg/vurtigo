#ifndef DICOMREADERUI_H
#define DICOMREADERUI_H

#include <QWidget>
#include <QString>
#include <ui_DICOMReader.h>

#include <vtkSmartPointer.h>

#include "DICOMFileReader.h"

class DICOMReaderUI : public QWidget, public Ui::DICOMReader
{
  Q_OBJECT

public:
  DICOMReaderUI();
  ~DICOMReaderUI();

public slots:
  void newDirectory();
  void dirChooser();
  void saveAsVolume();
  void spacingChanged() {m_customReader.setSpacing(spacingX->value(),spacingY->value(),spacingZ->value());}
  void setupVolumes();

protected:
  void connectSignals();

  QString m_lastDir;

  DICOMFileReader m_customReader;

  //! The ID of the newest volume
  int m_vol;

  //! The volume is a DICOM
  bool isDICOM;
};

#endif // DICOMREADERUI_H
