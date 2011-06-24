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
