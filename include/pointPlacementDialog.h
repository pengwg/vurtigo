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
