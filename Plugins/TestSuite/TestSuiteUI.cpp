/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include <iostream>

#include <QFileDialog>
#include <QString>

#include "TestSuiteUI.h"

TestSuiteUI::TestSuiteUI()
{
  setupUi(this);

  m_basicTest = new TestSuiteBasic();

  resultsEdit->setPlainText("");

  setupSlots();
}

TestSuiteUI::~TestSuiteUI() {
  if (m_basicTest) {
    m_basicTest->exit();
    m_basicTest->wait(1000);
    delete m_basicTest;
  }
}


void TestSuiteUI::setupSlots() {
  connect(basicPush, SIGNAL(clicked()), this, SLOT(basicTest()));
  connect(benchPush, SIGNAL(clicked()), this, SLOT(benchTest()));
  connect(clearPush, SIGNAL(clicked()), this, SLOT(clearTextArea()));

  connect( m_basicTest, SIGNAL(sendOutput(QString)), this, SLOT(addText(QString)), Qt::QueuedConnection );
}

bool fExist(QString *filename)
  {
    FILE *f = fopen(filename->toStdString().c_str(), "rb");
    fclose(f);
    
    return (f != NULL);
  }

void TestSuiteUI::basicTest() {
  QString pngFile("/home/hawk/x.png");
  QString dicomFile("/home/hawk/x.dcm");
  
  if (!fExist(&pngFile))
    pngFile = QFileDialog::getOpenFileName(this, "Select PNG File", QDir::currentPath(),"Image File (*.png)" );
    
  if (!fExist(&dicomFile)) 
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

void TestSuiteUI::addText(QString text) {
  resultsEdit->append(text);
}
