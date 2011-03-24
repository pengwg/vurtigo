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
  virtual bool addToRenderer(vtkRenderer* ren);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren);
  virtual bool getObjectLocation(double loc[6]);

  virtual void setRenderQuality(double quality);

public slots:
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);
  virtual void wheelEvent(QWheelEvent* event);

  virtual void newDataAvailable();

  virtual void resetAxialPlane();
  virtual void resetSagittalPlane();
  virtual void resetCoronalPlane();

  virtual void setBoundBox(bool);

  virtual void movePlanes(bool,bool,bool);
  virtual void moveAxialNormal(double);
  virtual void moveAxialAxial(double);
  virtual void moveSagittalNormal(double);
  virtual void moveSagittalSagittal(double);
  virtual void moveCoronalNormal(double);
  virtual void moveCoronalCoronal(double);

 protected:
  void setupDataObject();
  void setupPipeline();
  void update3PlaneStatus();

  void update();

  vtkSmartPointer<vtkImageReslice> m_transFilter;

  vtkSmartPointer<vtkVolumeRayCastMapper> m_rayMapper;
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

};

#endif
