#ifndef RT_WINDOW_LEVEL_DIALOG_H
#define RT_WINDOW_LEVEL_DIALOG_H

#include "ui_rtWindowLevelDialog.h"

#include <QDialog>
#include <QFile>
#include <QList>
#include <QGraphicsScene>

#include "vtkImageData.h"


class rtWindowLevelDialog : public QDialog, public Ui::rtWindowLevelDialog {
  Q_OBJECT
  public:

  const unsigned int c_histogramSize;

    struct WLObject {
      QString name;
      double window;
      double level;
      double old_window;
      double old_level;
      bool modified;
    };

    rtWindowLevelDialog(QString fileName="");
    ~rtWindowLevelDialog();

    void setImageData(vtkImageData* dat);

    void updateRangeBars(double w, double l);

  public slots:
    void newLine();
    void saveLine();
    void saveAll();

    void windowSliderChange(int);
    void levelSliderChange(int);

    void selectNewRow(int row, int col, int old_row, int old_col);
    void changeCell(int row, int col);

  signals:
    void valuesChanged(int window, int level);
  protected:
    double m_defWindow;
    double m_defLevel;

    double m_currWindow;
    double m_currLevel;

    QString m_fName;

    QList<WLObject> m_levelList;
    int m_currSelection;
    double m_range[2];

    QGraphicsScene* m_scene;
    QPen m_linePen;
    int *m_histogram;

    void loadFile();
    void updateTable();
};

#endif
