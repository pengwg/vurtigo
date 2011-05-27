#include "HelloWorldDataObject.h"
#include "rtApplication.h"
#include "rtMessage.h"
#include "rtBaseHandle.h"

//! Constructor
HelloWorldDataObject::HelloWorldDataObject()
{
    setObjectType("HelloWorld");
    m_message = "Hello World!";
    setupGUI();
}

//! Destructor
HelloWorldDataObject::~HelloWorldDataObject()
{
    cleanupGUI();
}

void HelloWorldDataObject::update()
{

}

//! Set the GUI widgets
void HelloWorldDataObject::setupGUI()
{
    m_optionsWidget.setupUi(getBaseWidget());
    connect(m_optionsWidget.messageText, SIGNAL(textChanged(QString)), this, SLOT(messageChanged(QString)));
}

//! Clean the GUI widgets
void HelloWorldDataObject::cleanupGUI()
{

}

//! Save the object to a file
bool HelloWorldDataObject::saveFile(QFile *file)
{
    if (!file->open(QIODevice::WriteOnly)) {
      rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for writing. Error Code: ").append(QString::number(file->error())));
      return false;
    }
    QXmlStreamWriter writer(file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("VurtigoFile");
    rtDataObject::saveHeader(&writer, getObjectType(), getObjName());

    ////////////////////////////////
    // WRITE OBJECT SPECIFIC DATA HERE
    writer.writeTextElement("message",m_message);
    ////////////////////////////////


    writer.writeEndElement(); // vurtigofile
    writer.writeEndDocument();
    file->close();
    return true;
}

//! Load the object from a file
bool HelloWorldDataObject::loadFile(QFile *file)
{
    if (!file->open(QIODevice::ReadOnly)) {
      rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Failed to open file for reading. Error Code: ").append(QString::number(file->error())));
      return false;
    }
    QXmlStreamReader reader(file);
    QString objType;
    QString objName="";
    while (!reader.atEnd()) {
      if (reader.readNext() == QXmlStreamReader::StartElement )
      {
        if (reader.name() == "FileInfo")
        {
          rtDataObject::loadHeader(&reader, objType, objName);
        }
        else if (reader.name() == "message")
        {
            m_optionsWidget.messageText->setText(reader.readElementText());
        }
      }
    }
    if (reader.hasError())
    {
      return false;
      rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("XML Reader Failed. Error: ").append(reader.errorString()));
    }
    Modified();
    return true;

}

//! The message has changed
void HelloWorldDataObject::messageChanged(const QString &text)
{
    m_message = text;
    Modified();
}
