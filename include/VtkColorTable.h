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
#ifndef VTK_COLOR_TABLE_H
#define VTK_COLOR_TABLE_H

#include <QVector>
#include <iostream>
using namespace std;

#include "ui_VtkColorTable.h"
#include <vtkColorTransferFunction.h>

//! Widget that represents a vtkColorTransferFunction, stores the points of the function as a QVector of structs. The points are sorted.
class VtkColorTable : public QWidget, protected Ui::VtkColorTable {
    Q_OBJECT
public:
    //! Represetnation of a point in the color function
    struct Point {
        int scalarValue;
        QColor color;
        // The midpoint and sharpness values specified when adding a node are used to control the transition to the next node
        // The midpoint is the normalized distance between control points at which the curve reaches the median Y value
        double midpoint;
        //A sharpness of 0 yields a piecewise linear function and a sharpness of 1 yields a piecewise constant function
        double sharpness;

        bool operator==(const Point & p1) {
            return p1.color == color && p1.scalarValue == scalarValue && p1.midpoint == midpoint && p1.sharpness == sharpness;
        }
    };

    //! Representation of the current state
    struct Profile {
        QVector<Point> points;
        bool clamping;
    };

    VtkColorTable(QWidget * parent = NULL);
    ~VtkColorTable();

    void getCurrentProfileState(Profile & profile);
    void loadProfile(const Profile & profile);
    void getDefaultProfile(Profile & profile);

    vtkColorTransferFunction * getColorFunction() { return colorFunction; }
    bool setColorFunction(vtkColorTransferFunction * const func);
signals:
    //! Called when the color function has changed.
    void functionUpdated();
private:
    vtkColorTransferFunction * colorFunction;

    //! Representation of the colorFunction points
    QVector<Point> points;
    //! The currently edited cell
    QPoint editingCell;

    void setupSignals();
    void updateFromPoints();
    void updateColors();

    void updateColorFunction();
    void removeCheckDupe(int oldScalarVal);

    void addTableRow(int row, const Point & point);
    void updateTableRow(int row, const Point & point);
private slots:
    void newPoint();
    void delPoint();

    void cellEditingSlot(int row, int column);

    void updateCell(int row, int column);
    void updateClamp(int state);

    void updateAll(const QColor & val);
    void updateRed(int val, bool updateAll = true);
    void updateGreen(int val, bool updateAll = true);
    void updateBlue(int val, bool updateAll = true);
};

#endif // VTK_COLOR_TABLE_H
