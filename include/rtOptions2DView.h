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
