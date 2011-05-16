#include "HelloWorldPluginUI.h"
#include "HelloWorldRenderObjectMaker.h"

#include "rtBaseHandle.h"
#include <QInputDialog>

HelloWorldPluginUI::HelloWorldPluginUI()
{
    setupUi(this);

    rtBaseHandle::instance().addNewObject("HelloWorld", new HelloWorldRenderObjectMaker());

    connectSignals();
}

HelloWorldPluginUI::~HelloWorldPluginUI()
{

}

void HelloWorldPluginUI::connectSignals()
{
    connect(newButton, SIGNAL(clicked()), this, SLOT(addNewObject()));
}

void HelloWorldPluginUI::addNewObject()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Get New Object Name"),
                                         tr("New object name:"), QLineEdit::Normal,
                                         "Hello World Object", &ok);
    if (ok && !text.isEmpty())
       m_helloWorldObjectID = rtBaseHandle::instance().requestNewObject("HelloWorld",text);
}
