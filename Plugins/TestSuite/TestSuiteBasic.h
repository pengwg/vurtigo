#ifndef TESTSUITEBASIC_H
#define TESTSUITEBASIC_H

#include <QThread>
#include <QTimer>

class TestSuiteBasic : public QThread
{
  Q_OBJECT

public:
  TestSuiteBasic();

  void run();

public slots:
  void changeImage();
signals:
  void sendOutput( QString );

protected:
  void basicTestCreateObjects();
  void testObject(int, QString);


  QTimer m_imgChange;
  int m_imgPeriod;

  // Object IDs
  int m_label;
  int m_3DPoints;
  int m_cath[3];
  int m_smallVol;
  int m_hugeVol;
  int m_2DPlane;
  int m_matrix;
  int m_polyObj;
};

#endif // TESTSUITEBASIC_H
