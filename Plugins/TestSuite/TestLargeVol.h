#ifndef TEST_LARGE_VOL_H
#define TEST_LARGE_VOL_H

#include <QThread>

class TestLargeVol : public QThread
{
  Q_OBJECT

public:
  TestLargeVol();
  ~TestLargeVol();

  void run();

public slots:

signals:
  void sendOutput( QString );

protected:
  void createObjects();
  void testObject(int, QString);

  // Object IDs
  int m_hugeVol;
};

#endif // TEST_LARGE_VOL_H
