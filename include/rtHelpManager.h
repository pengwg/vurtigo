#ifndef RTHELPMANAGER_H
#define RTHELPMANAGER_H

#include <QDialog>
#include <QHelpEngine>
#include <QTextBrowser>
#include <QByteArray>
#include <QUrl>
#include <iostream>
class rtHelpManager : public QObject
{
  Q_OBJECT
 public:

  //! Special Text Browser
  class rtHelpTextBrowser : public QTextBrowser {
    public:
    rtHelpTextBrowser(QHelpEngine* p = 0) {
      m_engine=p;
    }

  protected:
    QHelpEngine* m_engine;
  private:
    QVariant loadResource(int type, const QUrl &name) {
      QByteArray ba;
      if (type < 4 && m_engine) {
        QUrl url(name);
        if (name.isRelative())
          url = source().resolved(url);
        ba = m_engine->fileData(url);
      }
      return ba;
    }
  };

  rtHelpManager();
  ~rtHelpManager();
 public slots:
  void showHelpContents();
  void showSourceDocs();
  void showHelpAbout();

  void sourceLinkActivated(QUrl);

  void sourceEngineWarning(QString);
 protected:
  QDialog *m_helpSourceDialog;
  QHelpEngine *m_helpSourceEngine;
  rtHelpTextBrowser*  m_helpSourceBrowser;

};

#endif // RTHELPMANAGER_H
