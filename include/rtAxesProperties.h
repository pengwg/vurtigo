#ifndef RT_AXES_PROPERTIES_H
#define RT_AXES_PROPERTIES_H

#include "ui_axesProperties.h"

class QFile;

class rtAxesProperties : public QDialog, private Ui::axesProperties {
Q_OBJECT

 public:

  enum ViewType {
    VT_NONE,
    VT_VISIBLE,
    VT_INTERACT
  };

  enum CoordType {
    CT_HFS,
    CT_FFS,
    CT_HFP,
    CT_FFP,
    CT_HFDR,
    CT_FFDR,
    CT_HFDL,
    CT_FFDL
  };

  rtAxesProperties(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtAxesProperties();

  bool isChanged() { return m_changed; }

  ViewType getViewType() { return m_viewType; }
  CoordType getCoordType() { return m_coordType; }
  QString getMarkerStyle() { return markerStyleCombo->currentText(); }
  QString getMarkerPath() { return m_markerStylesToPaths.value(getMarkerStyle()); }
  void setCoordType(int ct);

 public slots:
  void settingsChanged();
  void markerPropSelectionChanged(QString);

 protected:
  //! True if the user clicked the OK button to apply the changes.
  bool m_changed;

  ViewType m_viewType;
  CoordType m_coordType;
  QMap<QString, QString> m_markerStylesToPaths;

  bool loadMarkerStylesFromConfig(QFile*);
};

#endif
