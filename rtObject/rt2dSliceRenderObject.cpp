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

// Vurtigo includes
#include "rt2dSliceRenderObject.h"
#include "rt2dSliceDataObject.h"
#include "rtObjectManager.h"
#include "rtApplication.h"
#include "rtMainWindow.h"

// VTK includes
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkCellArray.h>


/////////////////
// Public
/////////////////

rt2DSliceRenderObject::rt2DSliceRenderObject() {
  setObjectType("OT_2DObject");
  setName("2DSlice Renderer");
  // initialize!!!
  m_selectedProp = NULL;
  m_mousePos.setX(-1);
  m_mousePos.setY(-1);
  setupDataObject();
  setupPipeline();
}


rt2DSliceRenderObject::~rt2DSliceRenderObject() {

  while (!m_reslice.isEmpty()) {
    m_reslice.takeLast()->Delete();
  }

  while (!m_mapToColors.isEmpty()) {
    m_mapToColors.takeLast()->Delete();
  }

  while (!m_colorFunction.isEmpty()) {
    m_colorFunction.takeLast()->Delete();
  }

  m_imageBlend->Delete();

  m_imgCast->Delete();
  m_imgMap->Delete();
  m_actor2D->Delete();
}

bool rt2DSliceRenderObject::addToRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(window,true);

  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
  if (!dObj->isDataValid()) return false;

  // Update the object.
  dObj->Modified();
  this->tryUpdate();

  if(!ren->HasViewProp(m_texturePlane.getActor())) {
    ren->AddViewProp(m_texturePlane.getActor());
  }

  if(!ren->HasViewProp(m_boxOutline.getActor())) {
    ren->AddViewProp(m_boxOutline.getActor());
  }

  customQVTKWidget* renWid;
  renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(window);
  // Connect mouse actions
  connect(renWid, SIGNAL(interMousePress(QMouseEvent*,int)), this, SLOT(mousePressEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoveEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(mouseReleaseEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*,int)));
  connect(renWid, SIGNAL(interKeyPress(QKeyEvent*,int)), this, SLOT(keyPressEvent(QKeyEvent*,int)));
  connect(renWid, SIGNAL(interKeyRelease(QKeyEvent*,int)), this, SLOT(keyReleaseEvent(QKeyEvent*,int)));
  connect(renWid, SIGNAL(interWheel(QWheelEvent*,int)), this, SLOT(wheelEvent(QWheelEvent*,int)));
  return true;
}

bool rt2DSliceRenderObject::removeFromRenderer(vtkRenderer* ren,int window) {
  if (!ren) return false;
  setVisible3D(window,false);

  if(ren->HasViewProp(m_texturePlane.getActor())) {
    ren->RemoveViewProp(m_texturePlane.getActor());
  }

  if(ren->HasViewProp(m_boxOutline.getActor())) {
    ren->RemoveViewProp(m_boxOutline.getActor());
  }

  customQVTKWidget* renWid;
  renWid = rtApplication::instance().getMainWinHandle()->getRenderWidget(window);

  // Disconnect mouse actions
  disconnect(renWid, SIGNAL(interMousePress(QMouseEvent*,int)), this, SLOT(mousePressEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseMove(QMouseEvent*,int)), this, SLOT(mouseMoveEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseRelease(QMouseEvent*,int)), this, SLOT(mouseReleaseEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interMouseDoubleClick(QMouseEvent*,int)), this, SLOT(mouseDoubleClickEvent(QMouseEvent*,int)));
  disconnect(renWid, SIGNAL(interKeyPress(QKeyEvent*,int)), this, SLOT(keyPressEvent(QKeyEvent*,int)));
  disconnect(renWid, SIGNAL(interKeyRelease(QKeyEvent*,int)), this, SLOT(keyReleaseEvent(QKeyEvent*,int)));
  disconnect(renWid, SIGNAL(interWheel(QWheelEvent*,int)), this, SLOT(wheelEvent(QWheelEvent*,int)));

  return true;
}

bool rt2DSliceRenderObject::getObjectLocation(double loc[6]) {
  if (!m_boxOutline.getActor()) return false;

  m_boxOutline.getActor()->GetBounds(loc);

  return true;
}

//////////////////
// Public Slots
//////////////////

void rt2DSliceRenderObject::mousePressEvent(QMouseEvent* event,int window) {
  if (!m_selectedProp)
    {
      // if nothing is selected
       if (!rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->getChosenProp())
            rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMousePress(event,window);
      return;
  }
  if (m_control.isShowing()) {
      if (event->modifiers() == Qt::ShiftModifier)
      {
          m_mousePos.setX(event->globalX());
          m_mousePos.setY(event->globalY());
      }
      else
          m_control.mousePressEvent(event,window);
  }
}

void rt2DSliceRenderObject::mouseMoveEvent(QMouseEvent* event,int window) {
  if (!m_selectedProp)
    {
      // if nothing is selected
       if (!rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->getChosenProp())
            rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseMove(event,window);
      return;
  }
  if (m_control.isShowing()) {
      // if we are holding down the button while moving
      if ((event->modifiers() == Qt::ShiftModifier) && (m_mousePos.x() != -1))
      {
          rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
          int minw,maxw,val,minl,maxl,stepw,stepl;
          minw = dObj->getWLDialog()->windowSlider->minimum() / 1000;
          maxw = dObj->getWLDialog()->windowSlider->maximum() / 1000;
          minl = dObj->getWLDialog()->levelSlider->minimum() / 1000;
          maxl = dObj->getWLDialog()->levelSlider->maximum() / 1000;
          // find a step amount based on the range
          stepw = (maxw - minw) / 200;
          stepl = (maxl - minl) / 200;
          stepw = (stepw < 1)? 1 : stepw;
          stepl = (stepl < 1)? 1 : stepl;

          int w = dObj->getWindow() + (int)( - event->globalY() + m_mousePos.y())*stepw;
          if (w < 0)
              w = 0;
          int l = dObj->getLevel() + (int)(  event->globalX() - m_mousePos.x())*stepl;
          if (l < 0)
              l = 0;



          val = (w>maxw)?maxw:(w - minw);
          dObj->getWLDialog()->windowSlider->setValue(val*1000);
          dObj->getWLDialog()->windowSliderChange(val*1000);

          val = (l>maxl)?maxl:(l - minl);
          dObj->getWLDialog()->levelSlider->setValue(val*1000);
          dObj->getWLDialog()->levelSliderChange(val*1000);

          m_mousePos.setX(event->globalX());
          m_mousePos.setY(event->globalY());
      }
      else

          m_control.mouseMoveEvent(event,window);
  }
}

void rt2DSliceRenderObject::mouseReleaseEvent(QMouseEvent* event,int window) {
  if (!m_selectedProp)
    {
      // if nothing is selected
       if (!rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->getChosenProp())
            rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseRelease(event,window);
      return;
  }
  // reset the last mouse press to nothing
   m_mousePos.setX(-1);

  if (m_control.isShowing()) {
    vtkTransform *t = vtkTransform::New();
    rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
    m_control.mouseReleaseEvent(event,window);
    m_control.getTransform(t);
    dObj->setVtkMatrix(t->GetMatrix(), true);
    dObj->Modified();
    t->Delete();
  }
}

void rt2DSliceRenderObject::mouseDoubleClickEvent(QMouseEvent* event,int window) {
  vtkProp* temp;

  temp = rtApplication::instance().getMainWinHandle()->getSelectedProp(window);
  m_selectedProp = NULL;

  if (m_control.isShowing())
    m_control.hide();

  if (temp) {
    if (temp == m_texturePlane.getActor() || temp == m_boxOutline.getActor()) {
      m_selectedProp = temp;
      rtApplication::instance().getMainWinHandle()->getObjectTree()->setCurrentItem(this->getTreeItem());
      m_control.show();
    }
  }

  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);

  if (m_selectedProp) {
    dObj->setManualOn();
  } else {
    dObj->setManualOff();
  }
}

void rt2DSliceRenderObject::keyPressEvent(QKeyEvent* event,int window) {
  if (!m_selectedProp) return;
  if (event->key() == Qt::Key_D)
  {
      for (int ix1=0; ix1<rtApplication::instance().getMainWinHandle()->getNumRenWins(); ix1++)
      {
          rtApplication::instance().getMainWinHandle()->getRenderWidget(ix1)->setChosenProp(NULL);
      }
      if (m_control.isShowing())
      {
          m_control.hide();
      }
      m_selectedProp = NULL;
      rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
      dObj->setManualOff();


  }
}

void rt2DSliceRenderObject::keyReleaseEvent(QKeyEvent* event,int window) {
  if (!m_selectedProp) return;
}

void rt2DSliceRenderObject::wheelEvent(QWheelEvent* event,int window) {
  if (!m_selectedProp)
    {
      // if nothing is selected
       if (!rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->getChosenProp())
            rtApplication::instance().getMainWinHandle()->getRenderWidget(window)->camTakeOverMouseWheel(event,window);
      return;
  }
  if (m_control.isShowing()) {
    vtkTransform *t = vtkTransform::New();
    rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
    m_control.wheelEvent(event,window);
    m_control.getTransform(t);
    dObj->setVtkMatrix(t->GetMatrix(), true);
    dObj->Modified();
    t->Delete();
  }
}

/////////////////////////////
// Protected
////////////////////////////
void rt2DSliceRenderObject::setupDataObject() {
  m_dataObj = new rt2DSliceDataObject();
}


void rt2DSliceRenderObject::setupPipeline() {
  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);

    // Do the 2D planes as well.
  m_imgCast = vtkImageCast::New();
  m_imgMap = vtkImageMapper::New();
  m_actor2D = vtkActor2D::New();

  m_imgCast->SetOutputScalarTypeToUnsignedShort();

  m_imageBlend = vtkImageBlend::New();
  for (unsigned int ix1=0; ix1<dObj->maxNumberOfInputs(); ix1++) {
    m_reslice.append(vtkImageReslice::New());
    m_mapToColors.append(vtkImageMapToColors::New());
    m_colorFunction.append(vtkWindowLevelLookupTable::New());

    // Set the lookup table for each color function.
    m_mapToColors[ix1]->SetLookupTable(m_colorFunction[ix1]);
  }

  m_imgMap->SetInputConnection(m_imageBlend->GetOutputPort());
  m_imgMap->RenderToRectangleOn();
  m_actor2D->SetMapper(m_imgMap);
  m_pipe2D.insert("2DSlice", m_actor2D);
  m_canRender3D = true;
}

void rt2DSliceRenderObject::update() {
  rt2DSliceDataObject* dObj = static_cast<rt2DSliceDataObject*>(m_dataObj);
  if ( !dObj->isDataValid() || !dObj->getRawData() || !dObj->getUCharData()) return;

  double scaleRange_raw[2];
  double bounds[6];

  dObj->getRawData()->Update();
  dObj->getRawData()->GetScalarRange(scaleRange_raw);
  dObj->getRawData()->GetBounds(bounds);
      
  m_texturePlane.setImageData(dObj->getRawData());
  m_texturePlane.setScalarRange(scaleRange_raw[0], scaleRange_raw[1]);
  m_texturePlane.setWindow(dObj->getWindow());
  m_texturePlane.setLevel(dObj->getLevel());
  m_texturePlane.setSize(bounds[1]-bounds[0], bounds[3]-bounds[2]);
  m_texturePlane.setTransform(dObj->getTransform());
  m_texturePlane.getActor()->GetProperty()->SetOpacity(dObj->getOpacity());

  m_boxOutline.setSize(bounds[1]-bounds[0], bounds[3]-bounds[2]);
  m_boxOutline.setTransform(dObj->getTransform());

  m_control.setTransform(dObj->getTransform());
  m_control.setSize(bounds[1]-bounds[0], bounds[3]-bounds[2] );
  m_control.setObjectOfInterest(m_texturePlane.getActor());
  // Update the 2D as well
  m_imgCast->SetInput(dObj->getUCharData());

 // not sure if this old scaleRange code is needed anymore
  double scaleRange[2];
  dObj->getUCharData()->Update();
  dObj->getUCharData()->GetScalarRange(scaleRange);

  double range[2];
  int dims[3];

  rt2DSliceInputColorWidget* inputWidget;
  unsigned short connectionNum = 0;
  double tempColor[3];

  m_imageBlend->RemoveAllInputs();

  // Check the multiple inputs.
  for (unsigned short ix1=0; ix1<dObj->maxNumberOfInputs(); ix1++) {
    inputWidget = dObj->getColorWidgetAt(ix1);
    if ( inputWidget->getCurrentObjectID() == dObj->getId() ) {
      inputWidget->getCurrentColorAsVTKArray(tempColor);
      m_colorFunction[ix1]->SetRange(scaleRange[0], scaleRange[1]);
      m_colorFunction[ix1]->SetMinimumTableValue(0.0, 0.0, 0.0, 1.0);
      m_colorFunction[ix1]->SetMaximumTableValue(tempColor[0], tempColor[1], tempColor[2], 1.0);
      m_colorFunction[ix1]->Build();
      m_mapToColors[ix1]->SetInputConnection(m_imgCast->GetOutputPort());
      m_imageBlend->AddInput(0, m_mapToColors[ix1]->GetOutput());
      connectionNum++;
    } else if ( inputWidget->getCurrentObjectID() >= 0) {
      // TODO
      // Continue here by getting the objects with the right ID and then adding them to the m_reslice so that they volumes can be cut.
      //connectionNum++;
    }
  }

  for (unsigned short ix1=0; ix1<connectionNum; ix1++) {
    m_imageBlend->SetOpacity(ix1, 1.0f/static_cast<double>(connectionNum));
  }

  // If there are no connections then use a defualt.
  if (connectionNum == 0) {
    //m_colorFunction[0]->RemoveAllPoints();
    inputWidget->getCurrentColorAsVTKArray(tempColor);
    m_colorFunction[0]->SetMinimumTableValue(0.0, 0.0, 0.0, 1.0);
    m_colorFunction[0]->SetMaximumTableValue(1.0, 1.0, 1.0, 1.0);
    m_colorFunction[0]->Build();
    m_mapToColors[0]->SetInputConnection(m_imgCast->GetOutputPort());
    m_imageBlend->AddInput(0, m_mapToColors[0]->GetOutput());
    m_imageBlend->SetOpacity(0, 1.0);
  }

  m_imageBlend->GetOutput()->Update();
  m_imageBlend->GetOutput()->GetScalarRange(range);
  m_imageBlend->GetOutput()->GetDimensions(dims);

  double scalarRangeSize = (scaleRange[1]-scaleRange[0]);
  if (scalarRangeSize == 0.0) scalarRangeSize = 1.0;

  m_imgMap->SetColorWindow( dObj->getWindow()/scalarRangeSize*(range[1]-range[0]) );
  m_imgMap->SetColorLevel( dObj->getLevel()/scalarRangeSize*(range[1]-range[0]) );

  m_actor2D->SetPosition(0, 0);
  m_actor2D->SetPosition2(1, 1);

  // Set the render flag
  if ( rtApplication::instance().getMainWinHandle() ) {
    rtApplication::instance().getMainWinHandle()->setRenderFlag3D(true);
  }
}
