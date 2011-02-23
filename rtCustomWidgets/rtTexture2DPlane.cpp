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
#include <vtkMath.h>

#include "rtTexture2DPlane.h"

rtTexture2DPlane::rtTexture2DPlane() {
  // Objects for the texture pipeline.
  m_texturePlane = vtkPlaneSource::New();
  m_texture = vtkTexture::New();
  m_imgMapToColors = vtkImageMapToColors::New();
  m_lookupTable = vtkWindowLevelLookupTable::New();
  
  // Black and white.
  m_imgMapToColors->SetOutputFormatToRGB();

  m_lookupTable->SetHueRange(0.0, 0.0);
  m_lookupTable->SetSaturationRange(0.0, 0.0);
  m_lookupTable->SetValueRange(0.0, 1.0);

  m_texturePlane->SetOrigin(0,0,0);
  m_texturePlane->SetPoint1(1,0,0);
  m_texturePlane->SetPoint2(0,1,0);

  // Change the input for the mapper.
  m_mapper->SetInput(m_texturePlane->GetOutput());

  m_actor->SetTexture(m_texture);
  // For slices we don't need "real" lighting. Just Ambient.
  m_actor->GetProperty()->SetAmbient(1.0);
  m_actor->GetProperty()->SetDiffuse(0.0);
  m_actor->GetProperty()->SetSpecular(0.0);

  m_imgMapToColors->SetLookupTable(m_lookupTable);
}

rtTexture2DPlane::~rtTexture2DPlane() {
  if (m_texturePlane) m_texturePlane->Delete();
  if (m_texture) m_texture->Delete();
  if (m_imgMapToColors) m_imgMapToColors->Delete();
  if (m_lookupTable) m_lookupTable->Delete();
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
  if (img->GetNumberOfScalarComponents() == 1) {
    m_texture->SetInputConnection(m_imgMapToColors->GetOutputPort());
    m_imgMapToColors->SetInput(img);
  }
  else {
    m_texture->SetInput(img); 
  }
}

void rtTexture2DPlane::update() {
  m_texturePlane->SetOrigin(m_origin);
  m_texturePlane->SetPoint1(m_pt1);
  m_texturePlane->SetPoint2(m_pt2);
}
