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
