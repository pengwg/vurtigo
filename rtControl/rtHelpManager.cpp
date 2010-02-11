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
