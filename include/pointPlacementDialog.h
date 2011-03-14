#ifndef POINTPLACEMENTDIALOG_H
#define POINTPLACEMENTDIALOG_H

#include "ui_pointPlacement.h"

#include <QMouseEvent>
#include <QColor>



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

    void addPoint(QMouseEvent*);
    void addNewPoints();

    void placementOn();
    void placementOff();


protected:


    //! The list of 3D point objects
    QList<int> m_points;

    //! The current color
    QColor m_color;
    QStringList m_colorList;

};

#endif // POINTPLACEMENTDIALOG_H
