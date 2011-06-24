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
