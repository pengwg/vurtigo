#ifndef RT_LABEL_DATA_OBJECT_H
#define RT_LABEL_DATA_OBJECT_H

// Local Includes
#include "rtDataObject.h"

// VTK includes
#include "vtkTextProperty.h"

// Qt Includes
#include <QString>
#include <QLabel>
#include <QPushButton>

class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QLineEdit;

class rtLabelDataObject : public rtDataObject {

 Q_OBJECT

 public:
  rtLabelDataObject();
  ~rtLabelDataObject();

  void setText(QString text);
  QString getText();

  void setColor(double, double, double);
  void setOpacity(double);

  vtkTextProperty* getTextProperty();

  void apply();
  void update();

 public slots:
  void propertyDialog();

 protected:
  //Properties
  QString m_labelText;
  vtkTextProperty* m_textProp;

  //GUI elements

  //! The master vertical layout
  QGridLayout* m_masterLayout;
  //! Just a title label.
  QLabel* m_nameLabel;

  ////
  // Text Choice 
  ////
  //! Label for text choice
  QLabel m_chooseTextLabel;
  //! Edit box for text choice
  QLineEdit* m_chooseTextEdit;

  ////
  // Colour Choice
  ////
  QLabel m_propertyLabel;
  QPushButton m_propertyButton;

  // Functions
  void setupGUI();
  void cleanupGUI();

 private:
};

#endif
