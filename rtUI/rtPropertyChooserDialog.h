#ifndef RT_PROPERTY_CHOOSER_DIALOG_H
#define RT_PROPERTY_CHOOSER_DIALOG_H

#include "ui_propertyChooserDialog.h"
#include "vtkProperty.h"

class rtPropertyChooserDialog : public QDialog, private Ui::propertyChooserDialog {

Q_OBJECT
  
  public:
    rtPropertyChooserDialog(vtkProperty* initial, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~rtPropertyChooserDialog();

    bool isChanged() { return m_changed; }
 public slots:
  void settingsChanged();

  protected:
    vtkProperty* m_prop;

    //! True if the user clicked the OK button to apply the changes.
    bool m_changed;
};

#endif
