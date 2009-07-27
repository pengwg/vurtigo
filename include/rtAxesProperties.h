#ifndef RT_AXES_PROPERTIES_H
#define RT_AXES_PROPERTIES_H

#include "ui_axesProperties.h"

class rtAxesProperties : public QDialog, private Ui::axesProperties {
Q_OBJECT

 public:

  enum ViewType {
    VT_NONE,
    VT_VISIBLE,
    VT_INTERACT
  };

  enum CoordType {
    CT_PATIENT,
    CT_VTK
  };

  rtAxesProperties(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtAxesProperties();

  bool isChanged() { return m_changed; }

  ViewType getViewType() { return m_viewType; }
  CoordType getCoordType() { return m_coordType; }

 public slots:
  void settingsChanged();

 protected:
  //! True if the user clicked the OK button to apply the changes.
  bool m_changed;

  ViewType m_viewType;
  CoordType m_coordType;

};

#endif
