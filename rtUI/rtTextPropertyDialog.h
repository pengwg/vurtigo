#ifndef RT_TEXT_PROPERTY_DIALOG_H
#define RT_TEXT_PROPERTY_DIALOG_H

#include "ui_textPropertyDialog.h"
#include "vtkTextProperty.h"

class rtTextPropertyDialog : public QDialog, private Ui::textPropertyDialog {
  Q_OBJECT
 
 public:
  rtTextPropertyDialog(vtkTextProperty* initial, QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~rtTextPropertyDialog();

  //! Check if the property was changed.
  bool isChanged() { return m_changed; }

  //! Get the newly changed property.
  vtkTextProperty* getProperty() { return m_textProp; }

 public slots:
  void settingsChanged();
  void colorButton();

 protected:
  //! True if the user clicked the OK button to apply the changes.
  bool m_changed;

  //! The text property to be modified. 
  vtkTextProperty* m_textProp;

};

#endif
