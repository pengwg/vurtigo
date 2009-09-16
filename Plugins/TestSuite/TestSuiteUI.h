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
#ifndef TESTSUITEUI_H
#define TESTSUITEUI_H

#include <QWidget>
#include "ui_TestSuite.h"
#include <QString>
#include "TestSuiteBasic.h"

class TestSuiteUI : public QWidget, private Ui::TestSuite {

  Q_OBJECT

  public:
    TestSuiteUI();
    ~TestSuiteUI();

  public slots:
    void basicTest();
    void benchTest();
    void clearTextArea();

    void addText(QString);

  protected:
    void setupSlots();

    TestSuiteBasic* m_basicTest;
};

#endif // TESTSUITEUI_H
