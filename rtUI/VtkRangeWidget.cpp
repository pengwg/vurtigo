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
#include "VtkRangeWidget.h"

VtkRangeDialog::VtkRangeDialog(QWidget * parent, HoverPoints * hoverPoints) : QDialog(parent) {
    this->hoverPoints = hoverPoints;
    setupUi(this);

    connect(mainButtonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(refresh()));
    connect(mainButtonBox, SIGNAL(accepted()), this, SLOT(applyChanges()));
    connect(mainButtonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(applyChanges()));
}

void VtkRangeDialog::refresh() {
    const QRectF & currDataSpace = hoverPoints->getDataSpace();
    scalarLowerLineEdit->setText(QString::number(currDataSpace.left()));
    scalarUpperLineEdit->setText(QString::number(currDataSpace.right()));
}

void VtkRangeDialog::applyChanges() {
    //parse
    bool ok;
    QRectF dataSpace;
    double lower;
    double upper;

    const QRectF & currDataSpace = hoverPoints->getDataSpace();

    lower = scalarLowerLineEdit->text().toDouble(&ok);
    if (!ok) return;
    dataSpace.setLeft(lower);

    upper = scalarUpperLineEdit->text().toDouble(&ok);
    if (!ok) return;
    dataSpace.setRight(upper);

    //take same opacity range
    dataSpace.setTop(currDataSpace.top());
    dataSpace.setBottom(currDataSpace.bottom());

    hoverPoints->setDataSpace(dataSpace);
}

VtkRangeWidget::VtkRangeWidget(QWidget * parent, HoverPoints * hoverPoints) : QWidget(parent) {
    setupUi(this);;

    rangeDialog = new VtkRangeDialog(this, hoverPoints);
    rangeDialog->setWindowTitle(DIALOG_WINDOW_TITLE);
    connect(hoverPoints, SIGNAL(dataSpaceChanged(QRectF)), this, SLOT(labelUpdate(QRectF)));
    connect(scalarButton, SIGNAL(clicked()), this, SLOT(changeDialog()));

    labelUpdate(hoverPoints->getDataSpace());
}

VtkRangeWidget::~VtkRangeWidget() {
    delete rangeDialog;
}

void VtkRangeWidget::changeDialog() {
    rangeDialog->refresh();
    rangeDialog->show();
    rangeDialog->raise();
    rangeDialog->activateWindow();
}

void VtkRangeWidget::labelUpdate(const QRectF & dataSpace) {
    scalarLabel->setText(QString(SCALAR_MAIN_TEXT).append(QString::number(dataSpace.left())).append(SCALAR_TO_TEXT).append(QString::number(dataSpace.right())));
}
