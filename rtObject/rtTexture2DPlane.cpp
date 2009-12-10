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
#include <vtkImageData.h>

#include "rtTexture2DPlane.h"

rtTexture2DPlane::rtTexture2DPlane() {
  // Objects for the texture pipeline.
  m_texturePlane = vtkPlaneSource::New();
  m_planeMapper = vtkPolyDataMapper::New();
  m_textureActor = vtkActor::New();
  m_texture = vtkTexture::New();
  m_imgMapToColors = vtkImageMapToColors::New();
  m_lookupTable = vtkWindowLevelLookupTable::New();
  m_transform = vtkTransform::New();

  // Default bounds.
  m_bounds[0] = 0.0;
  m_bounds[1] = 1.0;
  m_bounds[2] = 0.0;
  m_bounds[3] = 1.0;
  m_bounds[4] = 0.0;
  m_bounds[5] = 1.0;

  // Balck and white.
  m_imgMapToColors->SetOutputFormatToLuminance();

  m_lookupTable->SetHueRange(0.0, 0.0);
  m_lookupTable->SetSaturationRange(0.0, 0.0);
  m_lookupTable->SetValueRange(0.0, 1.0);


  m_texturePlane->SetOrigin(0,0,0);
  m_texturePlane->SetPoint1(1,0,0);
  m_texturePlane->SetPoint2(0,1,0);

  m_planeMapper->SetInput(m_texturePlane->GetOutput());
  m_textureActor->SetMapper(m_planeMapper);
  m_textureActor->SetTexture(m_texture);
  m_texture->SetInputConnection(m_imgMapToColors->GetOutputPort());
  m_texture->SetLookupTable(m_lookupTable);
  m_imgMapToColors->SetLookupTable(m_lookupTable);
}

rtTexture2DPlane::~rtTexture2DPlane() {
  if (m_texturePlane) m_texturePlane->Delete();
  if (m_planeMapper) m_planeMapper->Delete();
  if (m_textureActor) m_textureActor->Delete();
  if (m_texture) m_texture->Delete();
  if (m_imgMapToColors) m_imgMapToColors->Delete();
  if (m_lookupTable) m_lookupTable->Delete();
  if (m_transform) m_transform->Delete();
}

void rtTexture2DPlane::setScalarRange(double min, double max) {
  m_lookupTable->SetRange(min, max);
}

void rtTexture2DPlane::setWindow(double w) {
  m_lookupTable->SetWindow(w);
}

void rtTexture2DPlane::setLevel(double l) {
  m_lookupTable->SetLevel(l);
}

void rtTexture2DPlane::setImageData(vtkImageData* img) {
  m_imgMapToColors->SetInput(img);
}

void rtTexture2DPlane::setTransform( vtkTransform* t ) {
  m_transform->DeepCopy(t);
}

void rtTexture2DPlane::setBounds( double bounds[6] ) {
  for (int ix1=0; ix1<6; ix1++) m_bounds[ix1] = bounds[ix1];
}

void rtTexture2DPlane::update() {
  double orig[3], pt1[3], pt2[3];
  double xsize, ysize, zsize;

  xsize = m_bounds[1]-m_bounds[0];
  ysize = m_bounds[3]-m_bounds[2];

  // The Z size should be one...
  zsize = m_bounds[5]-m_bounds[4];

  orig[0]=0.0; orig[1]=0.0; orig[2]=0.0;
  pt1[0]=1.0*xsize; pt1[1]=0.0; pt1[2]=0.0;
  pt2[0]=0.0; pt2[1]=1.0*ysize; pt2[2]=0.0;

  m_transform->TransformPoint(orig, orig);
  m_transform->TransformPoint(pt1, pt1);
  m_transform->TransformPoint(pt2, pt2);

  m_texturePlane->SetOrigin(orig);
  m_texturePlane->SetPoint1(pt1);
  m_texturePlane->SetPoint2(pt2);
}
