#ifndef RT_WINDOW_LEVEL_DIALOG_H
#define RT_WINDOW_LEVEL_DIALOG_H

#include "ui_rtWindowLevelDialog.h"

#include <QDialog>

#include "vtkImageData.h"

class rtWindowLevelDialog : public QDialog, public Ui::rtWindowLevelDialog {
  Q_OBJECT
  public:
    rtWindowLevelDialog(QString fileName);
    ~rtWindowLevelDialog();

    void setImageData(vtkImageData* dat);

  protected:
    int m_defWindow;
    int m_defLevel;

    vtkImageData *m_tempData;
};

#endif
