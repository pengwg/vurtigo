#include <iostream>

#include <QFileDialog>
#include <QString>

#include "TestSuiteUI.h"

TestSuiteUI::TestSuiteUI()
{
  setupUi(this);

  m_basicTest = new TestSuiteBasic();
  m_largeVolTest = new TestLargeVol();

  resultsEdit->setPlainText("");

  setupSlots();
}

TestSuiteUI::~TestSuiteUI() {
  if (m_basicTest) {
    m_basicTest->exit();
    m_basicTest->wait(1000);
    delete m_basicTest;
  }

  if (m_largeVolTest) {
    m_largeVolTest->exit();
    m_largeVolTest->wait(1000);
    delete m_largeVolTest;
  }
}


void TestSuiteUI::setupSlots() {
  connect(basicPush, SIGNAL(clicked()), this, SLOT(basicTest()));
  connect(benchPush, SIGNAL(clicked()), this, SLOT(benchTest()));
  connect(clearPush, SIGNAL(clicked()), this, SLOT(clearTextArea()));
  connect(largeVolPush, SIGNAL(clicked()), this, SLOT(largeVolTest()));

  connect( m_basicTest, SIGNAL(sendOutput(QString)), this, SLOT(addText(QString)), Qt::QueuedConnection );
  connect( m_largeVolTest, SIGNAL(sendOutput(QString)), this, SLOT(addText(QString)), Qt::QueuedConnection );
}

void TestSuiteUI::basicTest() {
  QString pngFile("/home/hawk/x.png");
  QString dicomFile("/home/hawk/x.dcm");
  
  if (!QFile::exists(pngFile))
    pngFile = QFileDialog::getOpenFileName(this, "Select PNG File", QDir::currentPath(),"Image File (*.png)" );
    
  if (!QFile::exists(dicomFile))
    dicomFile = QFileDialog::getOpenFileName(this, "Select DICOM File", QDir::currentPath(),"Image File (*.dcm)" );

  m_basicTest->setPngFileName(pngFile);
  m_basicTest->setDicomFileName(dicomFile);
  
  m_basicTest->start();
}

//! Benchmark testing
/*!
  @todo Implement benchmarking for Vurtigo.
  */
void TestSuiteUI::benchTest() {
  std::cout << "Not Implemented Yet.." << std::endl;
}

void TestSuiteUI::clearTextArea() {
  resultsEdit->setPlainText("");
}

void TestSuiteUI::largeVolTest() {
  m_largeVolTest->start();
}

void TestSuiteUI::addText(QString text) {
  resultsEdit->append(text);
}
