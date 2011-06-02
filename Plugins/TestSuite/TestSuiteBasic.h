#ifndef TESTSUITEBASIC_H
#define TESTSUITEBASIC_H

#include <QThread>
#include <QTimer>

#include <vtkImageMapToColors.h>
#include <vtkWindowLevelLookupTable.h>

class TestSuiteBasic : public QThread
{
  Q_OBJECT

public:
  TestSuiteBasic();
  ~TestSuiteBasic();

  void run();

  inline void setPngFileName(QString name) { m_pngFileName=name; }
  inline void setDicomFileName(QString name) { m_dicomFileName=name; }

public slots:
  void changeImage();
signals:
  void sendOutput( QString );

protected:
  void basicTestCreateObjects();
  void testObject(int, QString);

  QString m_pngFileName;
  QString m_dicomFileName;

  QTimer m_imgChange;
  int m_imgPeriod;

  // Object IDs
  int m_label;
  int m_3DPoints;
  int m_cath[3];
  int m_smallVol;
  int m_2DPlane;
  int m_2DPlaneColor;
  int m_2DPlane16;
  int m_matrix;
  int m_polyObj;
  int m_ctf, m_ctfGreen, m_ctfTraffic;
  int m_piece;
  
  vtkImageMapToColors       *m_imgMapToColors;
  vtkWindowLevelLookupTable *m_lookupTable;

};

#endif // TESTSUITEBASIC_H
