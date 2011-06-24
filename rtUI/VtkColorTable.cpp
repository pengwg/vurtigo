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
#include "VtkColorTable.h"
#include <QColorDialog>

//used for testing, just prints out colorFunction's values
#define VTK_TEST

void VtkColorTable::setupSignals() {
    connect(mainTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(cellEditingSlot(int,int)));
    connect(mainTable, SIGNAL(cellChanged(int,int)), this, SLOT(updateCell(int,int)));

    connect(clampCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateClamp(int)));

    connect(newButton, SIGNAL(clicked()), this, SLOT(newPoint()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(delPoint()));
}

VtkColorTable::VtkColorTable(QWidget * parent) : QWidget(parent) {
    setupUi(this);

    colorFunction =  vtkColorTransferFunction::New();

    editingCell = QPoint(-1, -1);
    setupSignals();

    //mainTable Aesthetics
    mainTable->setColumnWidth(0, 70);
    for (int a = 1; a <= 4; a++)
        mainTable->setColumnWidth(a, 40);
    mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
}

VtkColorTable::~VtkColorTable() {
    colorFunction->Delete();
}

//! Updates all the gui components
void VtkColorTable::updateAll(const QColor & val) {
  updateRed(val.red(), false);
  updateGreen(val.green(), false);
  updateBlue(val.blue(), false);

  updateColors();
  updateColorFunction();
}

//! Updates the colors in the cell and the color triangle
void VtkColorTable::updateColors() {
    const QColor & color = points.at(mainTable->currentRow()).color;
    mainTable->item(mainTable->currentRow(), 4)->setBackgroundColor(color);
}

void VtkColorTable::updateColorFunction() {
    const Point & point = points.at(mainTable->currentRow());

    colorFunction->RemovePoint(point.scalarValue);
    colorFunction->AddRGBPoint(point.scalarValue, point.color.red()/255.0f, point.color.green()/255.0f, point.color.blue()/255.0f,point.midpoint,point.sharpness);
    emit functionUpdated();
}

/*!
Removes the maintable->currentRow()'th point in the colorFunction, but checks if there's another point with the same scalar value. If one exists,
colorFunction will take one of those points
*/
void VtkColorTable::removeCheckDupe(int oldScalarVal) {
    int currentRow = mainTable->currentRow();

    colorFunction->RemovePoint(oldScalarVal);
    if (currentRow > 0 && points.at(currentRow - 1).scalarValue == oldScalarVal) {
        const Point & point = points.at(currentRow - 1);
        colorFunction->AddRGBPoint(point.scalarValue, point.color.red()/255.0f,
                                    point.color.green()/255.0f, point.color.blue()/255.0f,point.midpoint,point.sharpness);
    }
    else if (points.size() - 1 > currentRow && points.at(currentRow + 1).scalarValue == oldScalarVal) {
        const Point & point = points.at(currentRow + 1);
        colorFunction->AddRGBPoint(point.scalarValue, point.color.red()/255.0f,
                                    point.color.green()/255.0f, point.color.blue()/255.0f,point.midpoint,point.sharpness);
    }
    emit functionUpdated();
}

void VtkColorTable::updateRed(int val, bool updateAll) {
    mainTable->item(mainTable->currentRow(), 1)->setText(QString::number(val));

    Point point = points.at(mainTable->currentRow());
    QColor & color = point.color;
    color.setRed(val);
    points.replace(mainTable->currentRow(), point);

    if (updateAll) {
        updateColors();
        updateColorFunction();
    }
}

void VtkColorTable::updateGreen(int val, bool updateAll) {
    mainTable->item(mainTable->currentRow(), 2)->setText(QString::number(val));

    Point point = points.at(mainTable->currentRow());
    QColor & color = point.color;
    color.setGreen(val);
    points.replace(mainTable->currentRow(), point);

    if (updateAll) {
        updateColors();
        updateColorFunction();
    }
}

void VtkColorTable::updateBlue(int val, bool updateAll) {
    mainTable->item(mainTable->currentRow(), 3)->setText(QString::number(val));

    Point point = points.at(mainTable->currentRow());
    QColor & color = point.color;
    color.setBlue(val);
    points.replace(mainTable->currentRow(), point);

    if (updateAll) {
        updateColors();
        updateColorFunction();
    }
}

void VtkColorTable::addTableRow(int row, const Point & point) {
    mainTable->insertRow(row);

    QTableWidgetItem * newItem = new QTableWidgetItem(tr("%1").arg(point.scalarValue));
    mainTable->setItem(row, 0, newItem);

    newItem = new QTableWidgetItem(tr("%1").arg(point.color.red()));
    mainTable->setItem(row, 1, newItem);

    newItem = new QTableWidgetItem(tr("%1").arg(point.color.green()));
    mainTable->setItem(row, 2, newItem);

    newItem = new QTableWidgetItem(tr("%1").arg(point.color.blue()));
    mainTable->setItem(row, 3, newItem);

    newItem = new QTableWidgetItem();
    newItem->setBackgroundColor(point.color);
    newItem->setFlags(Qt::ItemIsEnabled);
    mainTable->setItem(row, 4, newItem);

    newItem = new QTableWidgetItem(tr("%1").arg(point.midpoint));
    mainTable->setItem(row, 5, newItem);

    newItem = new QTableWidgetItem(tr("%1").arg(point.sharpness));
    mainTable->setItem(row, 6, newItem);
}

void VtkColorTable::updateTableRow(int row, const Point & point) {
    mainTable->item(row, 1)->setText(tr("%1").arg(point.color.red()));
    mainTable->item(row, 2)->setText(tr("%1").arg(point.color.green()));
    mainTable->item(row, 3)->setText(tr("%1").arg(point.color.blue()));
    mainTable->item(row, 4)->setBackgroundColor(point.color);
    mainTable->item(row, 5)->setText(tr("%1").arg(point.midpoint));
    mainTable->item(row, 6)->setText(tr("%1").arg(point.sharpness));
}

//! Updates everything based on the points
void VtkColorTable::updateFromPoints() {
    while (points.size() < mainTable->rowCount())
        mainTable->removeRow(mainTable->rowCount() - 1);

    colorFunction->RemoveAllPoints();
    for (int a = 0; a < points.size(); a++) {
        const Point & currPoint = points.at(a);
        if (a == mainTable->rowCount())
            addTableRow(a, currPoint);
        else
            updateTableRow(a, currPoint);

        colorFunction->AddRGBPoint(currPoint.scalarValue, currPoint.color.red()/255.0f,
                                   currPoint.color.green()/255.0f, currPoint.color.blue()/255.0f,currPoint.midpoint,currPoint.sharpness);
    }
    emit functionUpdated();
}

//! Used to keep track which cell the user is editing
void VtkColorTable::cellEditingSlot(int row, int column) {
    if (row < 0 || column < 0 || row >= mainTable->rowCount() || column > 6 ) return;

    if (column == 4) {
      // At this point we change the color.
      QColor col = QColorDialog::getColor(points.at(row).color, this);
      if (col.isValid()) {
        updateAll(col);
      }
    } else {
      editingCell = QPoint(row, column);
    }
}

inline static bool point_sort(const VtkColorTable::Point &p1, const VtkColorTable::Point &p2) {
    return p1.scalarValue < p2.scalarValue;
}

//! Updates components if the user has edited a cell
void VtkColorTable::updateCell(int row, int column) {
    //if the cell is edited by a user
    if (editingCell == QPoint(row, column)) {
        editingCell = QPoint(-1, -1);

        bool ok;
        uint val = mainTable->currentItem()->text().toUInt(&ok);
        // scalarValue
        if (column == 0) {
            //if the new value is invalid, take back the old value
            if (!ok) {
                mainTable->currentItem()->setText(QString::number(points.at(row).scalarValue));
            }
            else {
                //take the new value
                Point point = points.at(row);

                //update colorFunction
                removeCheckDupe(point.scalarValue);
                colorFunction->AddRGBPoint(val, point.color.red()/255.0f,
                                           point.color.green()/255.0f, point.color.blue()/255.0f,point.midpoint,point.sharpness);

                //add the point back in
                point.scalarValue = val;
                points.remove(row);
                points.append(point);

                qStableSort(points.begin(), points.end(), point_sort);

                emit functionUpdated();

                //match the table with the points
                mainTable->removeRow(row);
                int currentRow = points.lastIndexOf(point);
                addTableRow(currentRow, point);
                mainTable->setCurrentCell(currentRow, 0);
            }
        }
        // midpoint
        else if (column == 5)
        {
            bool okD;
            double valD = mainTable->currentItem()->text().toDouble(&okD);
            //if the new value is invalid, take back the old value
            if (!okD || valD < 0.0 || valD > 1.0)
                mainTable->currentItem()->setText(QString::number(points.at(row).midpoint));
            else
            {
                //take the new value
                Point point = points.at(row);
                //update colorFunction
                colorFunction->AddRGBPoint(point.scalarValue, point.color.red()/255.0f,
                                           point.color.green()/255.0f, point.color.blue()/255.0f,valD,point.sharpness);
                //add the point back in
                point.midpoint = valD;
                points.remove(row);
                points.append(point);
                qStableSort(points.begin(), points.end(), point_sort);
                emit functionUpdated();

                //match the table with the points
                mainTable->removeRow(row);
                int currentRow = points.lastIndexOf(point);
                addTableRow(currentRow, point);
                mainTable->setCurrentCell(currentRow, 0);
            }
        }
        // sharpness
        else if (column == 6)
        {
            bool okD;
            double valD = mainTable->currentItem()->text().toDouble(&okD);
            //if the new value is invalid, take back the old value
            if (!okD || valD < 0.0 || valD > 1.0)
                mainTable->currentItem()->setText(QString::number(points.at(row).sharpness));
            else
            {
                //take the new value
                Point point = points.at(row);
                //update colorFunction
                colorFunction->AddRGBPoint(point.scalarValue, point.color.red()/255.0f,
                                           point.color.green()/255.0f, point.color.blue()/255.0f,point.midpoint,valD);
                //add the point back in
                point.sharpness = valD;
                points.remove(row);
                points.append(point);
                qStableSort(points.begin(), points.end(), point_sort);
                emit functionUpdated();

                //match the table with the points
                mainTable->removeRow(row);
                int currentRow = points.lastIndexOf(point);
                addTableRow(currentRow, point);
                mainTable->setCurrentCell(currentRow, 0);
            }
        } else {
            //if the new value is invalid, take back the old value
            if (!ok || val > 255) {
                switch (column) {
                    case 1:
                        mainTable->currentItem()->setText(QString::number(points.at(row).color.red()));
                        break;
                    case 2:
                        mainTable->currentItem()->setText(QString::number(points.at(row).color.green()));
                        break;
                    case 3:
                        mainTable->currentItem()->setText(QString::number(points.at(row).color.blue()));
                }
            }
            else {
                switch (column) {
                    //update the appropriate color
                    case 1:
                        updateRed(val);
                        break;
                    case 2:
                        updateGreen(val);
                        break;
                    case 3:
                        updateBlue(val);
                }
            }
        }
    }
    else {
        editingCell = QPoint(-1, -1);
    }
}

void VtkColorTable::updateClamp(int state) {
    if (state == Qt::Unchecked)
        colorFunction->ClampingOff();
    else
        colorFunction->ClampingOn();

    emit functionUpdated();
}

void VtkColorTable::newPoint() {
    Point point;
    int currentRow = mainTable->currentRow();
    //copy the current highlighted point
    if (mainTable->rowCount() > 0) {

        // Check if there is a current row.
        if(currentRow == -1)
          currentRow = mainTable->rowCount()-1;

        const Point & currPoint = points.at(currentRow);
        point.scalarValue = currPoint.scalarValue;
        point.color = currPoint.color;
        point.midpoint = currPoint.midpoint;
        point.sharpness = currPoint.sharpness;
    }
    else {
    //otherwise take the default (zeroes)
        point.scalarValue = 0;
        point.color = QColor(0,0,0);
        point.midpoint = 0.5;
        point.sharpness = 0.0;
    }

    //update points
    points.append(point);
    qStableSort(points.begin(), points.end(), point_sort);

    //update colorFunction
    colorFunction->AddRGBPoint(point.scalarValue, point.color.red()/255.0f,
                               point.color.green()/255.0f, point.color.blue()/255.0f,point.midpoint,point.sharpness);

    //update table
    currentRow = points.lastIndexOf(point);
    addTableRow(currentRow, point);
    mainTable->setCurrentCell(currentRow, 0);

    emit functionUpdated();
}

void VtkColorTable::delPoint() {
    int currentRow = mainTable->currentRow();

    // Ensure that there is at least one row to remove.
    if (mainTable->rowCount() <= 0) return;

    // Set the current row if none exists
    if(currentRow == -1)
          currentRow = mainTable->rowCount()-1;

    removeCheckDupe(points.at(currentRow).scalarValue);

    points.remove(currentRow);
    
    mainTable->removeRow(currentRow);

    //if there are no more points, make a new one
    if (mainTable->rowCount() == 0) {
        newPoint();
    }
    //highlight the a row near the deleted row
    if (currentRow == mainTable->rowCount())
        mainTable->setCurrentCell(currentRow - 1, 0);
    else
        mainTable->setCurrentCell(currentRow, 0);
}

//! Returns the current profile state
void VtkColorTable::getCurrentProfileState(Profile & profile) {
    profile.points = points;
    profile.clamping = colorFunction->GetClamping() ? true : false;
}

void VtkColorTable::loadProfile(const Profile & profile) {
    points = profile.points;
    if (profile.clamping) {
        colorFunction->ClampingOn();
        clampCheckBox->setCheckState(Qt::Checked);
    }
    else {
        colorFunction->ClampingOff();
        clampCheckBox->setCheckState(Qt::Unchecked);
    }
    updateFromPoints();
    emit functionUpdated();
}

void VtkColorTable::getDefaultProfile(Profile & profile) {
    profile.points.clear();

    Point point;
    point.scalarValue = 0;
    point.color = QColor(0, 0, 0);
    point.midpoint = 0.5;
    point.sharpness = 0.0;

    profile.points.append(point);
    profile.clamping = false;
}

bool VtkColorTable::setColorFunction(vtkColorTransferFunction * const func) {
    if (!func || colorFunction == func) return false;

    colorFunction->DeepCopy(func);
    points.clear();
    double vals[6];
    for (int a = 0; a < colorFunction->GetSize(); a++) {
        colorFunction->GetNodeValue(a,vals);
        Point point;
        point.scalarValue = vals[0];
        point.color = QColor(vals[1]*255.0f, vals[2]*255.0f, vals[3]*255.0f);
        point.midpoint = vals[4];
        point.sharpness = vals[5];
        points.append(point);
    }

    updateFromPoints();
    emit functionUpdated();
    return true;
}
