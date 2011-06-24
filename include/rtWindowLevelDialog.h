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
