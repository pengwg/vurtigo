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
#include <QHBoxLayout>
#include <QHelpContentWidget>
#include <QHelpContentModel>
#include <QHelpIndexWidget>

#include <iostream>
#include <sstream>

#include "rtHelpManager.h"
#include "ui_rtAboutDialog.h"
#include "rtConfigOptions.h"
#include "buildParam.h"
#include "rtMessage.h"
#include "rtApplication.h"

rtHelpManager::rtHelpManager() {
  m_helpSourceDialog = new QDialog();
  m_helpSourceDialog->setWindowTitle("Source Code Help");
  QHBoxLayout* helpSourceLayout = new QHBoxLayout();
  m_helpSourceDialog->setLayout(helpSourceLayout);
  m_helpSourceEngine = new QHelpEngine(rtApplication::instance().getConfigOptions()->getSourceCodeHelpFileName(), m_helpSourceDialog);

  connect(m_helpSourceEngine, SIGNAL(warning(QString)), this, SLOT(sourceEngineWarning(QString)));

  if (!m_helpSourceEngine->setupData()) {     
    std::stringstream msg;
    msg << "Help Source File not Found at: " << rtApplication::instance().getConfigOptions()->getSourceCodeHelpFileName().toStdString();
    rtApplication::instance().getMessageHandle()->error(__LINE__, __FILE__, msg.str());
  }

  m_helpSourceBrowser = new rtHelpTextBrowser(m_helpSourceEngine);

  helpSourceLayout->addWidget(m_helpSourceEngine->contentWidget());
  helpSourceLayout->addWidget(m_helpSourceBrowser);

  connect (m_helpSourceEngine->contentWidget(), SIGNAL(linkActivated(QUrl)), this, SLOT(sourceLinkActivated(QUrl)));
}

rtHelpManager::~rtHelpManager() {
  if (m_helpSourceEngine) delete m_helpSourceEngine;
  if (m_helpSourceDialog) delete m_helpSourceDialog;
}

void rtHelpManager::showHelpContents() {
}

void rtHelpManager::showSourceDocs() {
  if (!m_helpSourceDialog) return;

  m_helpSourceDialog->show();
}

void rtHelpManager::showHelpAbout() {
  Ui::rtAboutDialog setup;
  QDialog aboutD;
  QString ver;

  ver.append("Version: ");
  ver.append(QString::number(VURTIGO_MAJOR_VERSION));
  ver.append(".");
  ver.append(QString::number(VURTIGO_MINOR_VERSION));
  ver.append(".");
  ver.append(QString::number(VURTIGO_REVISION_VERSION));

  setup.setupUi(&aboutD);
  setup.versionLabel->setText( ver );
  connect(setup.aboutCloseButton, SIGNAL(clicked()), &aboutD, SLOT(accept()));
  aboutD.exec();
}

void rtHelpManager::sourceLinkActivated(QUrl url) {
  if (m_helpSourceEngine) {
     m_helpSourceBrowser->setSource(url);
  }
}

void rtHelpManager::sourceEngineWarning(QString w) {
  std::cout << "Warning: " << w.toStdString() << std::endl;
}
