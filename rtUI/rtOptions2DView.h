#ifndef RT_OPTIONS_2D_VIEW
#define RT_OPTIONS_2D_VIEW

#include "ui_options2DView.h"
#include "customQVTKWidget.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"

#include <QWidget>
#include <QBoxLayout>
#include <QResizeEvent>

class rtOptions2DView : public QWidget, private Ui::options2DView {

Q_OBJECT

public:
  rtOptions2DView(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtOptions2DView();

  virtual QSize sizeHint();

  void setContainer(QWidget* w) { m_container=w; }

public slots:
  virtual void resizeEvent ( QResizeEvent * event );
protected:

  virtual void moveEvent ( QMoveEvent * event );

  customQVTKWidget* m_renderWidget;
  QHBoxLayout* m_renLayout;

  QWidget* m_container;

  vtkRenderWindowInteractor *m_inter2D;
  vtkRenderWindow* m_renWin2D;
  vtkRenderer* m_renderer2D;
};

#endif
