#include "ObjectControlWidget.h"

ObjectControlWidget::ObjectControlWidget() {
  m_showing = false;
  m_size = 1.0;
  m_transform = vtkTransform::New();
  m_transform->Identity();
}

ObjectControlWidget::~ObjectControlWidget() {
  m_transform->Delete();
}

void ObjectControlWidget::setTransform(vtkTransform* input) {
  m_transform->DeepCopy(input);
}

void ObjectControlWidget::getTransform(vtkTransform* output) {
  output->DeepCopy(m_transform);
}

void ObjectControlWidget::show() {
  if(m_showing) return;
  m_showing = true;
}

void ObjectControlWidget::hide() {
  if(!m_showing) return;
  m_showing = false;
}

bool ObjectControlWidget::isShowing() {
  return m_showing;
}

void ObjectControlWidget::mousePressEvent(QMouseEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::mouseMoveEvent(QMouseEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::mouseReleaseEvent(QMouseEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::keyPressEvent(QKeyEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::keyReleaseEvent(QKeyEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::wheelEvent(QWheelEvent* event) {
  if(!m_showing) return;

}

void ObjectControlWidget::updateWidgetPosition() {
}
