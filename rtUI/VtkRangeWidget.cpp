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
