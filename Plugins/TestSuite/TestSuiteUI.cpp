#include "TestSuiteUI.h"
#include <iostream>

TestSuiteUI::TestSuiteUI()
{
  setupUi(this);

  m_basicTest = new TestSuiteBasic();

  resultsEdit->setPlainText("");

  setupSlots();
}

TestSuiteUI::~TestSuiteUI() {
  if (m_basicTest) delete m_basicTest;
}


void TestSuiteUI::setupSlots() {
  connect(basicPush, SIGNAL(clicked()), this, SLOT(basicTest()));
  connect(benchPush, SIGNAL(clicked()), this, SLOT(benchTest()));
  connect(clearPush, SIGNAL(clicked()), this, SLOT(clearTextArea()));

  connect( m_basicTest, SIGNAL(sendOutput(QString)), this, SLOT(addText(QString)), Qt::QueuedConnection );
}


void TestSuiteUI::basicTest() {
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

void TestSuiteUI::addText(QString text) {
  resultsEdit->append(text);
}
