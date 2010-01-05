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

