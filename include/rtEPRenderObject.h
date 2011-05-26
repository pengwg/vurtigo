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
#ifndef RT_EP_RENDER_OBJECT_H
#define RT_EP_RENDER_OBJECT_H

#include "rtRenderObject.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>


//! The render object for EP data.
/*!
  Renders custom EP data.
  */
class rtEPRenderObject : public rtRenderObject {

 public:
  //! Constructor
  rtEPRenderObject();

  //! Destructor
  ~rtEPRenderObject();

  //! Add this object to the given renderer.
  virtual bool addToRenderer(vtkRenderer* ren,int window);

  //! Remove this object from the given renderer.
  virtual bool removeFromRenderer(vtkRenderer* ren,int window);
  virtual bool getObjectLocation(double loc[6]);

 protected:
  void setupDataObject();
  void setupPipeline();

  void update();

 private:
  vtkPolyDataMapper* m_pointMapper;
  vtkPolyDataMapper* m_meshMapper;
  vtkPolyDataMapper* m_infoMapper;
  vtkActor* m_pointActor;
  vtkActor* m_meshActor;
  vtkActor* m_infoActor;
};

#endif
