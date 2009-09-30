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

        bool operator==(const Point & p1) {
            return p1.color == color && p1.scalarValue == scalarValue;
        }
    };

    //! Representation of the current state
    struct Profile {
        QVector<Point> points;
        bool clamping;
    };

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
public:
    VtkColorTable(QWidget * parent = NULL);
    ~VtkColorTable();

    void getCurrentProfileState(Profile & profile);
    void loadProfile(const Profile & profile);
    void getDefaultProfile(Profile & profile);

    vtkColorTransferFunction * getColorFunction() { return colorFunction; }
    bool setColorFunction(vtkColorTransferFunction * const func);
};

#endif // VTK_COLOR_TABLE_H
