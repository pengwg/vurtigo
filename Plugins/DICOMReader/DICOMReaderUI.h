#ifndef DICOMREADERUI_H
#define DICOMREADERUI_H

#include <QWidget>
#include <QString>
#include <ui_DICOMReader.h>

#include <vtkDICOMImageReader.h>
#include <vtkSmartPointer.h>

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

protected:
  void connectSignals();

  QString m_lastDir;
  vtkSmartPointer<vtkDICOMImageReader> m_imageReader;

  //! The ID of the newest volume
  int m_vol;
};

#endif // DICOMREADERUI_H
