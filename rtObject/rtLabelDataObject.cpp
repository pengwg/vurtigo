#include "rtLabelDataObject.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QColorDialog>

rtLabelDataObject::rtLabelDataObject() {
  setObjectType(rtConstants::OT_TextLabel);
  m_labelText = "Sample Text";
  m_textProp = vtkTextProperty::New();
  setupGUI();
}

rtLabelDataObject::~rtLabelDataObject() {
  if (m_textProp) m_textProp->Delete();
  cleanupGUI();
}

//! Create the elements for the GUI
void rtLabelDataObject::setupGUI() {
  QWidget* wid = getBaseWidget();
  QPalette btnColor;
  double Cr,Cg,Cb;

  m_masterLayout = new QGridLayout();
  wid->setLayout(m_masterLayout);

  m_nameLabel = new QLabel("Text Label Options:", wid);
  m_masterLayout->addWidget(m_nameLabel, 0, 0);

  m_chooseTextLabel.setText("Display Text: ");
  m_chooseTextEdit = new QLineEdit();
  m_masterLayout->addWidget(&m_chooseTextLabel, 1, 0);
  m_masterLayout->addWidget(m_chooseTextEdit, 1, 1);

  m_chooseColorLabel.setText("Choose Colour: ");
  m_colorButton.setText("");
  m_textProp->GetColor(Cr, Cg, Cb);
  btnColor.setColor(QPalette::Button, QColor(Cr*255,Cg*255,Cb*255));
  m_colorButton.setPalette(btnColor);
  connect(&m_colorButton, SIGNAL(pressed()), this, SLOT(chooseColor()));

  m_masterLayout->addWidget(&m_chooseColorLabel, 2, 0);
  m_masterLayout->addWidget(&m_colorButton, 2, 1);

  m_masterLayout->addWidget(getApplyButton(), 3, 1);
}

void rtLabelDataObject::cleanupGUI() {

  if (m_masterLayout) delete m_masterLayout;
  if (m_nameLabel) delete m_nameLabel;

  if (m_chooseTextEdit) delete m_chooseTextEdit;
}

//! Take the info from the GUI
void rtLabelDataObject::apply() {
  m_labelText = m_chooseTextEdit->text();
  Modified();
}

//! Send the info to the GUI
void rtLabelDataObject::update() {
  m_chooseTextEdit->setText(m_labelText);
}

void rtLabelDataObject::setText(QString text) {
  m_labelText = text;
}

QString rtLabelDataObject::getText() {
  return m_labelText;
}


void rtLabelDataObject::setColor(double r, double g, double b) {
  if (m_textProp) {
    m_textProp->SetColor(r, g, b);
  }
}
  
void rtLabelDataObject::setOpacity(double opac) {
  if (m_textProp) {
    m_textProp->SetOpacity(opac);
  }
}

vtkTextProperty* rtLabelDataObject::getTextProperty() {
  return m_textProp;
}

//! Choose the text color
void rtLabelDataObject::chooseColor() {
  QColor temp;
  QPalette btnColor;
  temp = QColorDialog::getColor(m_colorButton.palette().color(QPalette::Button));
  if (temp.isValid()) {
    btnColor.setColor(QPalette::Button, temp);
    m_colorButton.setPalette(btnColor);
    this->setColor(temp.red()/255.0f, temp.green()/255.0f, temp.blue()/255.0f);
  }
}
