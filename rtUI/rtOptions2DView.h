#ifndef RT_OPTIONS_2D_VIEW
#define RT_OPTIONS_2D_VIEW

#include "ui_options2DView.h"
#include "customQVTKWidget.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkProp.h"
#include "vtkInteractorStyleRubberBand2D.h"

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

  vtkRenderWindowInteractor *m_inter2D;
  vtkRenderWindow* m_renWin2D;
  vtkRenderer* m_renderer2D;
  vtkProp* m_currProp;
  vtkInteractorStyleRubberBand2D* m_interactor;
  rtRenderObject* m_currRenObj;

  bool m_selected;
  bool m_renderFlag;
};

#endif
