#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QtGui/QDialog>
#include <QHash>

#include "rtNamedInfoPointData.h"

namespace Ui {
    class FilterDialog;
}

class FilterDialog : public QDialog {
  Q_OBJECT
public:
  FilterDialog(QWidget *parent = 0);
  ~FilterDialog();

  void setPointList(QHash<int, rtNamedInfoPointData>* ptList);

public slots:
  void filterButton();
  void cancelButton();
  void filterOpChanged(int op);
  void filterByChanged(int by);

protected:
  void changeEvent(QEvent *e);

  void setupCategories();
  void resetMinMaxValues();

private:
  Ui::FilterDialog *m_ui;
  QHash<int, rtNamedInfoPointData>* m_ptList;

  QString m_possibleOps[3];
};

#endif // FILTERDIALOG_H
