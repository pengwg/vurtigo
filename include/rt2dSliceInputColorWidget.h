#ifndef RT2DSLICEINPUTCOLORWIDGET_H
#define RT2DSLICEINPUTCOLORWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QColorDialog>
#include <QColor>
#include <QMap>

#include <vtkImageMapToColors.h>

//! Widget for the 2D slice type object that is repeated in the GUI.
class rt2DSliceInputColorWidget : public QWidget
{
Q_OBJECT

public:
  //! Constructor
  rt2DSliceInputColorWidget(int sliceObjectID);

  //! Destructor
  ~rt2DSliceInputColorWidget();

  //! Get the currently selected color
  QColor getCurrentColor() { return m_currentColor; }

  //! Get the currently selected color as a double array of three components
  void getCurrentColorAsVTKArray(double color[3]);

  //! Get the ID of the current object.
  int getCurrentObjectID() { return m_currentObjectID; }

  //! Set the ID for the current input.
  void setCurrentObjectID(int id);

public slots:
  //! The index of the input item has changed.
  void itemIndexChanged(int);

  //! The color chooser button was pressed
  void colorButtonPressed();

  //! New vurtigo object was created
  void newVurtigoObject(int);

  //! Vurtigo object was deleted
  void removedVurtigoObject(int);

signals:
  //! The color of the input has changed
  void inputColorModified(QColor);
  //! The id of the input has changed
  void inputIdModified(int);
protected:

  //! Remove previous entries and recreate the dropdown list from the object manager.
  /*!
    Removes all previous elements in the dropdown list and in the m_dropdownToObjectId map. Populates the map and the dropdown by:
    First adds the "None" with a value of -1. Next adds the current 2D object with the corresponding ID and finally adds a list of 3D type objects as given by the object manager.
    */
  void fillDropdownList();

  //! The id value of the current slice object.
  int m_sliceObjectID;

  //! Flag set when the lists are being updated.
  bool m_updatingLists;

  //! Horizontal layout for the widget
  QHBoxLayout m_widgetLayout;

  //! Combobox of possible input items.
  QComboBox m_itemName;

  //! The color chooser push button
  QPushButton m_colorButton;

  //! The current color that is being used.
  QColor m_currentColor;

  //! The value of the current object id.
  int m_currentObjectID;

  //! Maps location values in the dropdown list to object ids.
  QMap<int,int> m_dropdownToObjectId;
};

#endif // RT2DSLICEINPUTCOLORWIDGET_H
