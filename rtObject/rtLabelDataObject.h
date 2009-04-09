#ifndef RT_LABEL_DATA_OBJECT_H
#define RT_LABEL_DATA_OBJECT_H

#include "rtDataObject.h"
#include <QString>
#include "vtkTextProperty.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;

class rtLabelDataObject : public rtDataObject {
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

 protected:
  //Properties
  QString m_labelText;
  vtkTextProperty* m_textProp;

  //GUI elements

  //! The master vertical layout
  QVBoxLayout* m_vbox;
  //! Just a title label.
  QLabel* m_nameLabel;

  //! Layout for text choice
  QHBoxLayout* m_chooseTextLayout;
  //! Widget for text choice
  QWidget* m_chooseTextWidget;
  //! Label for text choice
  QLabel* m_chooseTextLabel;
  //! Edit box for text choice
  QLineEdit* m_chooseTextEdit;

  // Functions
  void setupGUI();
  void cleanupGUI();

 private:
};

#endif
