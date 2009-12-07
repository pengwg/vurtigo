/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
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
