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
#include "rtRenderOptions.h"

#include "vtkRenderWindow.h"

rtRenderOptions::rtRenderOptions() {
  setupUi(this);

  m_stereoTypes.insert("NONE", 0);
  m_stereoTypes.insert("Crystal Eyes", VTK_STEREO_CRYSTAL_EYES);
  m_stereoTypes.insert("Red Blue", VTK_STEREO_RED_BLUE);
  m_stereoTypes.insert("Interlaced", VTK_STEREO_INTERLACED);
  m_stereoTypes.insert("Left", VTK_STEREO_LEFT);
  m_stereoTypes.insert("Right", VTK_STEREO_RIGHT);
  m_stereoTypes.insert("Dresden", VTK_STEREO_DRESDEN);
  m_stereoTypes.insert("Anaglyph", VTK_STEREO_ANAGLYPH);
  m_stereoTypes.insert("Checkerboard", VTK_STEREO_CHECKERBOARD);

  QList<QString> types = m_stereoTypes.keys();

  stereoCombo->clear();
  for (int ix1=0; ix1<types.size(); ix1++) {
    stereoCombo->addItem(types.value(ix1));
  }

  connect(renQualSlider, SIGNAL(valueChanged(int)), this, SLOT(renQuality(int)));
  connect(renUpQualSlider, SIGNAL(valueChanged(int)), this, SLOT(renUpdateQuality(int)));
  connect(stereoCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(stereoComboChanged(QString)));
}

rtRenderOptions::~rtRenderOptions() {

}

void rtRenderOptions::removeStereoFeature() {
  stereoCombo->setDisabled(true);
}

void rtRenderOptions::renQuality(int qual) {
  if (renQualSlider->value() != qual) {
    renQualSlider->setValue(qual);
  }

  renQualLabel->setText(QString::number(qual) + QString(" % "));
  m_renQuality = qual;
}

void rtRenderOptions::renUpdateQuality(int qual) {
  if (renUpQualSlider->value() != qual) {
    renUpQualSlider->setValue(qual);
  }

  renUpQualLabel->setText(QString::number(qual) + QString(" % "));
  m_renUpdateQuality = qual;
}


void rtRenderOptions::setStereoType(int t) {
  if (t<0) return;
  m_currStereo = t;

  int loc = stereoCombo->findText(m_stereoTypes.key(t));
  stereoCombo->setCurrentIndex(loc);

}


void rtRenderOptions::setDirectRender(bool ren) {
  if (ren) {
    directRenderLabel->setText("Direct Rendering:   YES");
  } else {
    directRenderLabel->setText("Direct Rendering:   NO");
  }
}

void rtRenderOptions::setGLRender(bool ren) {
    if (ren) {
    glRenderLabel->setText("Can Use OpenGL Rendering:   YES");
  } else {
    glRenderLabel->setText("Can Use OpenGL Rendering:   NO");
  }
}


void rtRenderOptions::stereoComboChanged(QString t) {
  if (stereoCombo->currentText() != t) {
    int loc = stereoCombo->findText(t);
    stereoCombo->setCurrentIndex(loc);
  } else {
    m_currStereo = m_stereoTypes.value(t);
  }
}

