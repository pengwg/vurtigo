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
#ifndef RT_3D_VOLUME_RENDER_OBJECT_H
#define RT_3D_VOLUME_RENDER_OBJECT_H

#include "rtRenderObject.h"
#include "Plane2DControlWidget.h"
#include "rtBox2DOutline.h"
#include "rtTexture2DPlane.h"

#include <vtkImageCast.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkImageCast.h>
#include <vtkImageActor.h>
#include <vtkActor2D.h>
#include <vtkImageMapper.h>
#include <vtkImageReslice.h>
#include <vtkImageChangeInformation.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkSmartPointer.h>
#include <vtkImageResample.h>
#include <vtkTextActor.h>

//! The 3D Volume render object
/*!
  Rendering pipeline for the 3D object.
  */
class rt3DVolumeRenderObject : public rtRenderObject {

  Q_OBJECT

 public:

  rt3DVolumeRenderObject();
  ~rt3DVolumeRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren, int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren, int window);
  virtual bool getObjectLocation(double loc[6]);

  virtual void setRenderQuality(double quality);

  virtual bool hasProp(vtkProp *prop);

  //! Copy the information from another object
  virtual void copyObject(rtRenderObject *from);

  //! Copy the plane transformations from another Volume object
  void copyPlaneTransforms(rt3DVolumeRenderObject *from);

  //! Deselect this object
  virtual void deselect();

  // used for copying
  void setPlaneControlTransform(int plane, vtkTransform *trans) {m_planeControl[plane].setTransform(trans);}
  void setBoxOutlineTransform(int plane, vtkTransform *trans) {m_boxOutline[plane].setTransform(trans);}
  void setTexturePlaneTransform(int plane, vtkTransform *trans) {m_texturePlane[plane].setTransform(trans);}
  void getPlaneControlTransform(int plane, vtkTransform *trans) { m_planeControl[plane].getTransform(trans);}
  vtkTransform *getBoxOutlineTransform(int plane) { return m_boxOutline[plane].getTransform();}
  vtkTransform *getTexturePlaneTransform(int plane) { return m_texturePlane[plane].getTransform();}
  bool getBoxOutlineVisible(int plane) {return m_boxOutline[plane].getVisible();}
  bool getTexturePlaneVisible(int plane) {return m_texturePlane[plane].getVisible();}

public slots:
  virtual void mousePressEvent(QMouseEvent* event,int window);
  virtual void mouseMoveEvent(QMouseEvent* event, int window);
  virtual void mouseReleaseEvent(QMouseEvent* event, int window);
  virtual void mouseDoubleClickEvent(QMouseEvent* event, int window);
  virtual void wheelEvent(QWheelEvent* event, int window);

  virtual void newDataAvailable();

  virtual void resetAxialPlane();
  virtual void resetSagittalPlane();
  virtual void resetCoronalPlane();

  virtual void setBoundBox(bool);

  void setGPUOptions(bool);

  //! move the given planes to the move point
  virtual void movePlanes(bool,bool,bool);
  //! move the axial plane along its normal
  virtual void moveAxialNormal(double);
  //! move the axial plane along the axial direction
  virtual void moveAxialAxial(double);
  //! move the sagittal plane along its normal
  virtual void moveSagittalNormal(double);
  //! move the sagittal plane along the sagittal direction
  virtual void moveSagittalSagittal(double);
  //! move the coronal plane along its normal
  virtual void moveCoronalNormal(double);
  //! move the coronal plane along the coronal direction
  virtual void moveCoronalCoronal(double);

  //used for synching
  void syncWheel(int plane,QWheelEvent* event , int window);
  void hideWidget(int plane) {m_planeControl[plane].hide();}
  void showWidget(int plane) {m_planeControl[plane].setVisibilities(this->getVisible3D()); m_planeControl[plane].show();}
  int getCurrentPlane() {return m_currentPlane;}


 protected:
  void setupDataObject();
  void setupPipeline();
  void update3PlaneStatus();

  void update();

  vtkSmartPointer<vtkImageReslice> m_transFilter;

  vtkSmartPointer<vtkVolumeRayCastMapper> m_rayMapper;
  vtkSmartPointer<vtkGPUVolumeRayCastMapper> m_gpuRayMapper;
  vtkSmartPointer<vtkVolume> m_volumeActor;

  vtkSmartPointer<vtkOutlineFilter> m_outline;
  vtkSmartPointer<vtkPolyDataMapper> m_outlineMapper;
  vtkSmartPointer<vtkActor> m_outlineActor;

  vtkTextActor* m_annotateActor;

  ///////////////
  // The 2D planes
  /////////////////
  vtkImageCast* m_imgCast[3];
  vtkImageMapper* m_imgMap[3];
  vtkActor2D* m_actor2D[3];

  int m_currentPlane;

  Plane2DControlWidget m_planeControl[3];
  vtkImageReslice* m_imgReslice[3];

  rtBox2DOutline m_boxOutline[3];
  rtTexture2DPlane m_texturePlane[3];

  //! Check if the data has been initialized
  bool m_isInit;

  //! Adjust the reslice position with a given ID.
  void adjustReslice(int slice);

 private:
  //! The position (in 2D) of a mouse click
  QPoint m_mousePos;

};

#endif
