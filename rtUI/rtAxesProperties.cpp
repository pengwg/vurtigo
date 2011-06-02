#include "rtAxesProperties.h"
#include "rtApplication.h"
#include "rtConfigOptions.h"
#include "rtMessage.h"

#include <QDir>
#include <QFile>
#include <QXmlInputSource>
#include <QXmlSimpleReader>

class ModelsXmlHandler : public QXmlDefaultHandler
{
public:
  QMap<QString, QString> m_markerStylesAndLocations;

  ModelsXmlHandler() {};
  ~ModelsXmlHandler() {};

  bool startDocument() {
    m_markerStylesAndLocations.clear();
    return true;
  };

  bool startElement(const QString & namespaceURI,
                    const QString & localName,
                    const QString & qName,
                    const QXmlAttributes & atts)
  {
    if (localName == "MarkerStyle") {
      QString name = atts.value("name");
      QString path = rtApplication::instance().getConfigOptions()->getConfigPath() + "/models/" + atts.value("filename");
      QFile file(path);

      if (file.exists()) {
        m_markerStylesAndLocations.insert(name, path);
      } else {
        rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Xml document references a file that does not exist"));
      }
    }

    return true;
  };

  bool endElement(const QString & namespaceURI,
                  const QString & localName,
                  const QString & qName)
  {
    return true;
  };

  bool endDocument() {
    return true;
  };

};

rtAxesProperties::rtAxesProperties(QWidget *parent, Qt::WindowFlags flags) {
  setupUi(this);

  connect(this, SIGNAL(accepted()), this, SLOT(settingsChanged()));
  connect(markerStyleCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(markerPropSelectionChanged(QString)));

  m_changed=false;
  m_viewType = VT_VISIBLE;
  m_coordType = CT_FFS;

  visibleComboBox->setCurrentIndex(1);
  coordComboBox->setCurrentIndex(1);

  // The default marker style.
  m_markerStylesToPaths.insert("Labeled Axes", "DEFAULT");
  markerStyleCombo->addItem("Labeled Axes");

  // The absolute coordinate system axes.
  m_markerStylesToPaths.insert("XYZ", "XYZ");
  markerStyleCombo->addItem("XYZ");

  QFile file(rtApplication::instance().getConfigOptions()->getConfigPath() + "/DefMarkerStyles.xml");
  loadMarkerStylesFromConfig(&file);
}

//! Destructor
rtAxesProperties::~rtAxesProperties() {
}

//! Set the coordinate axes
void rtAxesProperties::setCoordType(int ct)
{
    coordComboBox->setCurrentIndex(ct);
    settingsChanged();
}

//! The user has clicked on OK so the changes will be made.
void rtAxesProperties::settingsChanged() {
  m_changed=true;
  int vt, ct;

  vt =  visibleComboBox->currentIndex();
  ct = coordComboBox->currentIndex();

  switch (vt) {
  case 0:
    m_viewType = VT_NONE;
    break;
  case 1:
    m_viewType = VT_VISIBLE;
    break;
  case 2:
    m_viewType = VT_INTERACT;
    break;  
  }

  switch (ct) {
  case 0:
    m_coordType = CT_HFS;
    break;
  case 1:
    m_coordType = CT_FFS;
    break;
  case 2:
    m_coordType = CT_HFP;
    break;
  case 3:
    m_coordType = CT_FFP;
    break;
  case 4:
    m_coordType = CT_HFDR;
    break;
  case 5:
    m_coordType = CT_FFDR;
    break;
  case 6:
    m_coordType = CT_HFDL;
    break;
  case 7:
    m_coordType = CT_FFDL;
    break;
  }

}

void rtAxesProperties::markerPropSelectionChanged(QString txt)
{
  coordComboBox->setEnabled(txt != "XYZ");
}

bool rtAxesProperties::loadMarkerStylesFromConfig(QFile *file)
{
  if (!file->exists()) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Unable to locate file"));
    return false;
  }

  QXmlInputSource source(file);
  ModelsXmlHandler handler;
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);

  if (!reader.parse(&source)) {
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, QString("Unable to parse file"));
    return false;
  }

  QMapIterator<QString, QString> itr(handler.m_markerStylesAndLocations);
  while (itr.hasNext()) {
    itr.next();
    m_markerStylesToPaths.insert(itr.key(), itr.value());
    markerStyleCombo->addItem(itr.key());
  }

  return true;
}
