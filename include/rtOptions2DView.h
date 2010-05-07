/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#ifndef RT_OPTIONS_2D_VIEW
#define RT_OPTIONS_2D_VIEW

#include "ui_options2DView.h"
#include "customQVTKWidget.h"

#include <vtkActor2D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkProp.h>
#include <vtkProperty2D.h>

#include <QWidget>
#include <QBoxLayout>
#include <QResizeEvent>
#include <QHash>
#include <QString>

class rtRenderObject;

class rtOptions2DView : public QWidget, private Ui::options2DView {

Q_OBJECT

public:
  rtOptions2DView(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtOptions2DView();

  virtual QSize sizeHint();

  void setContainer(QWidget* w) { m_container=w; }
  void setStringList(QHash<int, QString> *textList);

  inline void setRenderFlag(bool flag) { m_renderFlag=flag; }
  inline void renderOn() { m_renderFlag = true; }
  inline void renderOff() { m_renderFlag = false; }
  inline bool getRenderFlag() { return m_renderFlag; }

  inline vtkProp* getCurrProp() { return m_currProp; }
  inline rtRenderObject* getCurrRenObj() { return m_currRenObj; }

  inline bool isSelected() { return m_selected; }
  inline void setSelected(bool sel) { m_selected = sel; }

  void tryRender();

public slots:
  virtual void resizeEvent ( QResizeEvent * event );
  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  virtual void comboIndexChanged(int index);

protected:

  virtual void moveEvent ( QMoveEvent * event );

  customQVTKWidget* m_renderWidget;
  QHBoxLayout* m_renLayout;

  QWidget* m_container;

  vtkRenderWindow* m_renWin2D;
  vtkRenderer* m_renderer2D;
  vtkProp* m_currProp;
  rtRenderObject* m_currRenObj;

  bool m_selected;
  bool m_renderFlag;
};

#endif
