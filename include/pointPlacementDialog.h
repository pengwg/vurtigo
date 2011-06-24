/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#ifndef POINTPLACEMENTDIALOG_H
#define POINTPLACEMENTDIALOG_H

#include "ui_pointPlacement.h"

#include <QMouseEvent>
#include <QColor>

class rtRenderObject;
//! A Dialog for Point Placement
class pointPlacementDialog : public QDialog, private Ui::pointPlacement {

Q_OBJECT

  public:
    pointPlacementDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~pointPlacementDialog();

public slots:
    // set the names of the 3D point objects in the combo boxes
    void setupCombo();
    // setup the table values with points
    void setupTable();

    void addPoint(QMouseEvent*,int);
    void addNewPoints();

    void placementOn();
    void placementOff();

    void mouseMoved(QMouseEvent*,int);

    void tableChanged(int,int);
    void changeColor(int,int);

    void comboChanged();


protected:


    //! The list of 3D point objects
    QList<int> m_points;

    //! The currently selected point object
    rtRenderObject *m_currPoints;

    //! The current color
    QColor m_color;
    QStringList m_colorList;

    //! A flag to check whether the mouse moved
    bool m_moved;

};

#endif // POINTPLACEMENTDIALOG_H
