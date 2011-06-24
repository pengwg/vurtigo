/***************************************************************************
Vurtigo: Visualization software for interventional applications in medicine


Copyright (c) 2011, Sunnybrook Research Institute
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Sunnybrook Research Institute nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Sunnybrook Research Institute BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
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
