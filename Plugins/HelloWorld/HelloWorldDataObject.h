#ifndef HELLOWORLDDATAOBJECT_H
#define HELLOWORLDDATAOBJECT_H

#include "rtDataObject.h"
#include "ui_HelloWorldUI.h"

//! Object that represents the data
/*!
  Hello World Sample Data Object
  */
class HelloWorldDataObject : public rtDataObject
{
    Q_OBJECT

public:
    //! Constructor
    HelloWorldDataObject();
    //! Destructor
    ~HelloWorldDataObject();

    void update();
    //! Save the object to a file
    bool saveFile(QFile *file);
    //! Load the object from a file
    bool loadFile(QFile *file);

    QString getMessage() { return m_message; }
    void setMessage(QString msg) { m_message = msg; }

public slots:
    void messageChanged(const QString & text );

protected:
    // Functions
    //! Set the GUI widgets
    void setupGUI();
    //! Clean the GUI widgets
    void cleanupGUI();

    //member variables

    QString m_message;

    //! The widget for the options for this object
    Ui::HelloWorldUI m_optionsWidget;
};

#endif // HELLOWORLDDATAOBJECT_H
