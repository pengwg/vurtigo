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
#include "rtPropertyChooserDialog.h"
#include <QColorDialog>

//! Constructor to the dialog
rtPropertyChooserDialog::rtPropertyChooserDialog(vtkProperty* initial, QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  m_changed=false;
  m_prop = NULL;

  if (initial) {
    m_prop = initial;

    representComboBox->setCurrentIndex(m_prop->GetRepresentation());
    interpolateComboBox->setCurrentIndex(m_prop->GetInterpolation());

    double rgb[3];
    double edgeRgb[3];
    m_prop->GetColor(rgb);
    m_prop->GetEdgeColor(edgeRgb);

    redSpinBox->setValue(rgb[0]);
    greenSpinBox->setValue(rgb[1]);
    blueSpinBox->setValue(rgb[2]);

    redSpinBoxEdge->setValue(edgeRgb[0]);
    greenSpinBoxEdge->setValue(edgeRgb[1]);
    blueSpinBoxEdge->setValue(edgeRgb[2]);

    showEdgesGroupBox->setChecked(m_prop->GetEdgeVisibility());

    opacitySlider->setValue(m_prop->GetOpacity()*100.0f);
    ambientSlider->setValue(m_prop->GetAmbient()*100.0f);
    diffuseSlider->setValue(m_prop->GetDiffuse()*100.0f);
    specularSlider->setValue(m_prop->GetSpecular()*100.0f);
    specularPowerSlider->setValue(m_prop->GetSpecularPower()*100.0f);

    changeOpacity(m_prop->GetOpacity()*100.0f);
    changeAmbient(m_prop->GetAmbient()*100.0f);
    changeDiffuse(m_prop->GetDiffuse()*100.0f);
    changeSpecular(m_prop->GetSpecular()*100.0f);
    changeSpecularPower(m_prop->GetSpecularPower()*100.0f);

  } else {
    std::cout << "Error: rtPropertyChooserDialog.cpp: Initial Property was NULL!" << std::endl;
  }

  // Connect signals to slots
  connect(this, SIGNAL(accepted()), this, SLOT(settingsChanged()));
  connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));

  connect(representComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeRenderMode()));
  connect(interpolateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeRenderMode()));

  connect(colorChoosePushButton, SIGNAL(clicked()), this, SLOT(changeSurfaceColor()));
  connect(colorChoosePushButtonEdge, SIGNAL(clicked()), this, SLOT(changeEdgeColor()));

  connect(showEdgesGroupBox, SIGNAL(toggled(bool)), this, SLOT(changeEdgeVisibility(bool)));

  connect(redSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeSurfaceSpinBox()));
  connect(greenSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeSurfaceSpinBox()));
  connect(blueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeSurfaceSpinBox()));

  connect(redSpinBoxEdge, SIGNAL(valueChanged(double)), this, SLOT(changeEdgeSpinBox()));
  connect(greenSpinBoxEdge, SIGNAL(valueChanged(double)), this, SLOT(changeEdgeSpinBox()));
  connect(blueSpinBoxEdge, SIGNAL(valueChanged(double)), this, SLOT(changeEdgeSpinBox()));

  connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(changeOpacity(int)));
  connect(ambientSlider, SIGNAL(valueChanged(int)), this, SLOT(changeAmbient(int)));
  connect(diffuseSlider, SIGNAL(valueChanged(int)), this, SLOT(changeDiffuse(int)));
  connect(specularSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSpecular(int)));
  connect(specularPowerSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSpecularPower(int)));

}

rtPropertyChooserDialog::~rtPropertyChooserDialog() {

}

//! The user has clicked on OK so the changes to the vtkProperty will be made.
void rtPropertyChooserDialog::settingsChanged() {
  if (!m_prop) return;
  m_changed=true;

  m_prop->SetRepresentation( representComboBox->currentIndex() );
  m_prop->SetInterpolation( interpolateComboBox->currentIndex() );

  double rgb[3];
  double edgeRgb[3];

  rgb[0] = redSpinBox->value();
  rgb[1] = greenSpinBox->value();
  rgb[2] = blueSpinBox->value();

  edgeRgb[0] = redSpinBoxEdge->value();
  edgeRgb[1] = greenSpinBoxEdge->value();
  edgeRgb[2] = blueSpinBoxEdge->value();

  m_prop->SetColor(rgb);
  m_prop->SetEdgeColor(edgeRgb);

  m_prop->SetEdgeVisibility( showEdgesGroupBox->isChecked() );

  m_prop->SetOpacity(((double)opacitySlider->value())/100.0f);
  m_prop->SetAmbient(((double)ambientSlider->value())/100.0f);
  m_prop->SetDiffuse(((double)diffuseSlider->value())/100.0f);
  m_prop->SetSpecular(((double)specularSlider->value())/100.0f);
  m_prop->SetSpecularPower(((double)specularPowerSlider->value())/100.0f);

  emit propertyChanged();
}

void rtPropertyChooserDialog::buttonClicked(QAbstractButton* btn) {
  if (btn->text() == "Apply") {
    settingsChanged();
  } else if (btn->text() == "OK") {
    // This will be done automatically.
    //settingsChanged();
  }
}

void rtPropertyChooserDialog::changeRenderMode() {
  if (!m_prop) return;
  m_changed=true;
  m_prop->SetRepresentation( representComboBox->currentIndex() );
  m_prop->SetInterpolation( interpolateComboBox->currentIndex() );

  emit propertyChanged();
}

//! Change the surface color
void rtPropertyChooserDialog::changeSurfaceColor() {
  QColor oldColor;

  oldColor.setRed( redSpinBox->value() * 255.0f );
  oldColor.setGreen( greenSpinBox->value() * 255.0f );
  oldColor.setBlue( blueSpinBox->value() * 255.0f );

  QColor col = QColorDialog::getColor(oldColor);
  if (col.isValid()) {
    redSpinBox->setValue(((double)col.red())/255.0f);
    greenSpinBox->setValue(((double)col.green())/255.0f);
    blueSpinBox->setValue(((double)col.blue())/255.0f);

    double rgb[3];
    rgb[0] = redSpinBox->value();
    rgb[1] = greenSpinBox->value();
    rgb[2] = blueSpinBox->value();
    m_prop->SetColor(rgb);

    // Make the changes directly.
    m_changed=true;
    emit propertyChanged();
  }
}

//! Change the color of the polygon edges.
void rtPropertyChooserDialog::changeEdgeColor() {
  QColor oldColor;

  oldColor.setRed( redSpinBoxEdge->value() * 255.0f );
  oldColor.setGreen( greenSpinBoxEdge->value() * 255.0f );
  oldColor.setBlue( blueSpinBoxEdge->value() * 255.0f );

  QColor col = QColorDialog::getColor(oldColor);
  if (col.isValid()) {
    redSpinBoxEdge->setValue(((double)col.red())/255.0f);
    greenSpinBoxEdge->setValue(((double)col.green())/255.0f);
    blueSpinBoxEdge->setValue(((double)col.blue())/255.0f);

    double edgeRgb[3];

    edgeRgb[0] = redSpinBoxEdge->value();
    edgeRgb[1] = greenSpinBoxEdge->value();
    edgeRgb[2] = blueSpinBoxEdge->value();

    m_prop->SetEdgeColor(edgeRgb);

    // Make the changes directly.
    m_changed=true;
    emit propertyChanged();
  }
}

void rtPropertyChooserDialog::changeEdgeVisibility(bool v) {
  m_prop->SetEdgeVisibility(v);
  m_changed=true;
  emit propertyChanged();
}

void rtPropertyChooserDialog::changeSurfaceSpinBox() {
  double rgb[3];
  rgb[0] = redSpinBox->value();
  rgb[1] = greenSpinBox->value();
  rgb[2] = blueSpinBox->value();
  m_prop->SetColor(rgb);

  // Make the changes directly.
  m_changed=true;
  emit propertyChanged();
}

void rtPropertyChooserDialog::changeEdgeSpinBox() {
  double edgeRgb[3];

  edgeRgb[0] = redSpinBoxEdge->value();
  edgeRgb[1] = greenSpinBoxEdge->value();
  edgeRgb[2] = blueSpinBoxEdge->value();

  m_prop->SetEdgeColor(edgeRgb);

  // Make the changes directly.
  m_changed=true;
  emit propertyChanged();
}


void rtPropertyChooserDialog::changeOpacity(int opacity) {
  m_prop->SetOpacity(((double)opacity)/100.0f);
  opacityLabel->setText(QString::number(opacity)+" % ");

  // Make the changes directly.
  m_changed=true;
  emit propertyChanged();
}

void rtPropertyChooserDialog::changeAmbient(int ambient) {
  m_prop->SetAmbient(((double)ambient)/100.0f);
  ambientLabel->setText(QString::number(ambient)+" % ");

  // Make the changes directly.
  m_changed=true;
  emit propertyChanged();
}

void rtPropertyChooserDialog::changeDiffuse(int diffuse) {
  m_prop->SetDiffuse(((double)diffuse)/100.0f);
  diffuseLabel->setText(QString::number(diffuse)+" % ");

  // Make the changes directly.
  m_changed=true;
  emit propertyChanged();
}

void rtPropertyChooserDialog::changeSpecular(int specular) {
  m_prop->SetSpecular(((double)specular)/100.0f);
  specularLabel->setText(QString::number(specular)+" % ");

  // Make the changes directly.
  m_changed=true;
  emit propertyChanged();
}

void rtPropertyChooserDialog::changeSpecularPower(int specularPower) {
  m_prop->SetSpecularPower(((double)specularPower)/100.0f);
  specularPowerLabel->setText(QString::number(specularPower)+" % ");

  // Make the changes directly.
  m_changed=true;
  emit propertyChanged();
}
