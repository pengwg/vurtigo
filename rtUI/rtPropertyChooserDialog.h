#ifndef RT_PROPERTY_CHOOSER_DIALOG_H
#define RT_PROPERTY_CHOOSER_DIALOG_H

#include "ui_propertyChooserDialog.h"
#include "vtkProperty.h"

class rtPropertyChooserDialog : public QDialog, private Ui::propertyChooserDialog {

Q_OBJECT
  
  public:
    rtPropertyChooserDialog();
    ~rtPropertyChooserDialog();
};

#endif
