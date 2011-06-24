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
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkMath.h>
#include <vtkImageMapper.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageLuminance.h>

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

void rtTexture2DPlane::setImageData(vtkImageData* img, bool isGrey) {

    if (img->GetNumberOfScalarComponents() == 1)
    {
      m_imgMapToColors->SetInput(img);
      m_texture->SetInputConnection(m_imgMapToColors->GetOutputPort());

    }
    else
    {
        if (isGrey)
        {
            vtkSmartPointer<vtkImageLuminance> il = vtkSmartPointer<vtkImageLuminance>::New();
            il->SetInput(img);
            m_imgMapToColors->SetInput(il->GetOutput());
            m_texture->SetInputConnection(m_imgMapToColors->GetOutputPort());
        }
        else
        {
             m_texture->SetInput(img);
        }
    }


      /*! \todo {
        // trying to extract the 3 color components and window/level them individually

      vtkSmartPointer<vtkImageData> comp1 = vtkSmartPointer<vtkImageData>::New();
      vtkSmartPointer<vtkImageData> comp2 = vtkSmartPointer<vtkImageData>::New();
      vtkSmartPointer<vtkImageData> comp3 = vtkSmartPointer<vtkImageData>::New();
      vtkSmartPointer<vtkImageExtractComponents> extract = vtkSmartPointer<vtkImageExtractComponents>::New();
      vtkSmartPointer<vtkImageAppendComponents> append = vtkSmartPointer<vtkImageAppendComponents>::New();

      extract->SetInput(img);
      extract->SetComponents(0);
      m_imgMapToColors->SetInput(extract->GetOutput());
      comp1 = m_imgMapToColors->GetOutput();
      extract->SetComponents(1);
      m_imgMapToColors->SetInput(extract->GetOutput());
      comp2 = m_imgMapToColors->GetOutput();
      extract->SetComponents(2);
      m_imgMapToColors->SetInput(extract->GetOutput());
      comp3 = m_imgMapToColors->GetOutput();

      append->AddInput(comp1);
      append->AddInput(comp2);
      append->AddInput(comp3);

    m_texture->SetInput(append->GetOutput()); }
    */
}

void rtTexture2DPlane::update() {
  m_texturePlane->SetOrigin(m_origin);
  m_texturePlane->SetPoint1(m_pt1);
  m_texturePlane->SetPoint2(m_pt2);
}
