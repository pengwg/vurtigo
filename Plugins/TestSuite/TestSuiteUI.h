#ifndef TESTSUITEUI_H
#define TESTSUITEUI_H

#include <QWidget>
#include "ui_TestSuite.h"
#include <QString>

#include "TestSuiteBasic.h"
#include "TestLargeVol.h"

class TestSuiteUI : public QWidget, private Ui::TestSuite {

  Q_OBJECT

  public:
    TestSuiteUI();
    ~TestSuiteUI();

  public slots:
    void basicTest();
    void benchTest();
    void clearTextArea();

    //! Load a large volume.
    void largeVolTest();

    void addText(QString);

  protected:
    void setupSlots();

    TestSuiteBasic* m_basicTest;
    TestLargeVol* m_largeVolTest;
};

#endif // TESTSUITEUI_H
