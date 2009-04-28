#include "rtTextPropertyDialog.h"

#include <QString>
#include <QColorDialog>

#include <iostream>

rtTextPropertyDialog::rtTextPropertyDialog(vtkTextProperty* initial, QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  connect(this, SIGNAL(accepted()), this, SLOT(settingsChanged()));
  connect(colorPushButton, SIGNAL(pressed()), this, SLOT(colorButton()));

  m_changed=false;
  m_textProp = NULL;

  if (initial) {
    m_textProp = initial;

    fontComboBox->setCurrentIndex( m_textProp->GetFontFamily() );
    int idx = fontSizeComboBox->findText( QString::number(m_textProp->GetFontSize()) );
    if (idx >= 0) {
      fontSizeComboBox->setCurrentIndex( idx );
    }

    boldCheckBox->setChecked(false);
    italicCheckBox->setChecked(false);
    shadowCheckBox->setChecked(false);

    if (m_textProp->GetBold()) boldCheckBox->setChecked(true);
    if (m_textProp->GetItalic()) italicCheckBox->setChecked(true);
    if (m_textProp->GetShadow()) shadowCheckBox->setChecked(true);

    opacitySlider->setValue(m_textProp->GetOpacity()*100);
    
    double r,g,b;
    m_textProp->GetColor(r, g, b);
    redLineEdit->setText(QString::number(r*255));
    greenLineEdit->setText(QString::number(g*255));
    blueLineEdit->setText(QString::number(b*255));

    orientationDial->setValue( m_textProp->GetOrientation() );

  } else {
    std::cout << "Error: rtTextPropertyDialog.cpp: Initial Text Property was NULL!" << std::endl;
  }
}

rtTextPropertyDialog::~rtTextPropertyDialog() {

}

//! The user has clicked on OK so the changes to the vtkProperty will be made.
void rtTextPropertyDialog::settingsChanged() {
  if (!m_textProp) return;
  m_changed = true;

  m_textProp->SetFontFamily(fontComboBox->currentIndex());
  m_textProp->SetFontSize(fontSizeComboBox->currentText().toInt());
  if (boldCheckBox->isChecked()) {
    m_textProp->BoldOn();
  } else {
    m_textProp->BoldOff();
  }

  if (italicCheckBox->isChecked()) {
    m_textProp->ItalicOn();
  } else {
    m_textProp->ItalicOff();
  }

  if (shadowCheckBox->isChecked()) {
    m_textProp->ShadowOn();
  } else {
    m_textProp->ShadowOff();
  }

  m_textProp->SetOpacity(((double)opacitySlider->value())/100.0f);

  double r,g,b;
  r = ((double)redLineEdit->text().toInt())/255.0f;
  g = ((double)greenLineEdit->text().toInt())/255.0f;
  b = ((double)blueLineEdit->text().toInt())/255.0f;

  m_textProp->SetColor(r, g, b);
  m_textProp->SetOrientation(orientationDial->value());
}

void rtTextPropertyDialog::colorButton() {
  if (!m_textProp) return;
  QColor output;
  int r,g,b;
  r = redLineEdit->text().toInt();
  g = greenLineEdit->text().toInt();
  b = blueLineEdit->text().toInt();
  output = QColorDialog::getColor(QColor(r,g,b));
  if (output.isValid()) {
    redLineEdit->setText(QString::number(output.red()));
    greenLineEdit->setText(QString::number(output.green()));
    blueLineEdit->setText(QString::number(output.blue()));
  }
}
