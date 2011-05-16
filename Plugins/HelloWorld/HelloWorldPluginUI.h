#ifndef HELLOWORLDPLUGINUI_H
#define HELLOWORLDPLUGINUI_H

#include <QWidget>
#include <ui_HelloWorldPluginUI.h>

class HelloWorldPluginUI : public QWidget, public Ui::HelloWorldPluginUI
{
    Q_OBJECT
public:
    HelloWorldPluginUI();
    ~HelloWorldPluginUI();

public slots:
    void addNewObject();

protected:
    void connectSignals();

    //! The id of the newest object
    int m_helloWorldObjectID;
};

#endif // HELLOWORLDPLUGINUI_H
