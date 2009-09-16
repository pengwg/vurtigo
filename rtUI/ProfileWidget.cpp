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
#include "ProfileWidget.h"
#include <QDomDocument>
#include <QFile>

#include <iostream>

#define DOC_TYPE "Profile"
#define NAME_ATTR "name"
#define HOVERPOINTS_ELEMENT "HP_Profile"

#define POINTS_ELEMENT "points"
#define POINT_ELEMENT "point"
#define LOCK_ATTR "lock"
#define X_ATTR "x"
#define Y_ATTR "y"

#define DATASPACE_ELEMENT "dataSpace"
#define LEFT_ATTR "left"
#define RIGHT_ATTR "right"
#define TOP_ATTR "top"
#define BOTTOM_ATTR "bottom"

using namespace ProfileHandler;
using namespace std;

//! Given the dataspace DomNode, loads the dataspace from the attributes
inline bool loadDataSpace(HoverPoints::Profile & profile, const QDomNode & dataSpace) {
    QDomNamedNodeMap attr = dataSpace.attributes();
    bool ok;
    double left = attr.namedItem(LEFT_ATTR).nodeValue().toDouble(&ok);
    if (!ok)
        return false;
    double right = attr.namedItem(RIGHT_ATTR).nodeValue().toDouble(&ok);
    if (!ok)
        return false;
    double top = attr.namedItem(TOP_ATTR).nodeValue().toDouble(&ok);
    if (!ok)
        return false;
    double bottom = attr.namedItem(BOTTOM_ATTR).nodeValue().toDouble(&ok);
    if (!ok)
        return false;
    profile.dataSpace.setLeft(left);
    profile.dataSpace.setRight(right);
    profile.dataSpace.setTop(top);
    profile.dataSpace.setBottom(bottom);
    return true;
}

//! Given the points DomNode, loads the points the from the child nodes, defaults the lock as none if not provided
inline bool loadPoints(HoverPoints::Profile & profile, const QDomNode & points) {
    QDomNodeList pointsList = points.childNodes();
    profile.locks.resize(pointsList.size());
    profile.locks.fill(HoverPoints::NoLock);

    for (int a = 0; a < pointsList.size(); a++) {
        QDomNamedNodeMap attr = pointsList.at(a).attributes();
        bool ok;
        double x = attr.namedItem(X_ATTR).nodeValue().toDouble(&ok);
        if (!ok)
            return false;
        double y = attr.namedItem(Y_ATTR).nodeValue().toDouble(&ok);
        if (!ok)
            return false;

        profile.points << QPointF(x, y);
        if (attr.contains(LOCK_ATTR)) {
            unsigned int lock = attr.namedItem(LOCK_ATTR).nodeValue().toUInt(&ok);
            if (!ok)
                return false;
            profile.locks[a] = (HoverPoints::LockType)lock;
        }
    }
    return true;
}

//! Loads the profile into the parameter, given a filename
bool ProfileHandler::loadProfile(Profile & profile, const QString & filename) {
    QDomDocument doc(DOC_TYPE);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }

    QDomNodeList profileList = doc.elementsByTagName(DOC_TYPE);
    if (profileList.size() <= 0)
        return false;
    profile.name = profileList.at(0).attributes().namedItem(NAME_ATTR).nodeValue();
    if (profile.name == QString(""))
        profile.name = "Loaded unnamed profile";

    QDomNodeList hoverPointsList = doc.elementsByTagName(HOVERPOINTS_ELEMENT);
    if (hoverPointsList.size() <= 0)
        return false;

    QDomNode points = hoverPointsList.at(0).firstChildElement(POINTS_ELEMENT);
    if (points.isNull())
        return false;
    bool ok = loadPoints(profile.hoverPointsProfile, points);
    if (!ok)
        return false;

    QDomNode dataSpace = hoverPointsList.at(0).firstChildElement(DATASPACE_ELEMENT);
    if (dataSpace.isNull())
        return false;
    ok = loadDataSpace(profile.hoverPointsProfile, dataSpace);
    if (!ok)
        return false;

    return true;
}

//! Given a profile, saves the dataSpace as attibutes in the DomElement provided
inline void saveDataSpace(const HoverPoints::Profile & profile, QDomDocument & doc, QDomElement & dataSpace) {
    QDomAttr leftAttr = doc.createAttribute(LEFT_ATTR);
    leftAttr.setValue(QString::number(profile.dataSpace.left()));
    dataSpace.setAttributeNode(leftAttr);

    QDomAttr rightAttr = doc.createAttribute(RIGHT_ATTR);
    rightAttr.setValue(QString::number(profile.dataSpace.right()));
    dataSpace.setAttributeNode(rightAttr);

    QDomAttr topAttr = doc.createAttribute(TOP_ATTR);
    topAttr.setValue(QString::number(profile.dataSpace.top()));
    dataSpace.setAttributeNode(topAttr);

    QDomAttr bottomAttr = doc.createAttribute(BOTTOM_ATTR);
    bottomAttr.setValue(QString::number(profile.dataSpace.bottom()));
    dataSpace.setAttributeNode(bottomAttr);
}


//! Given a profile, saves the points as DomElements with attributes corresponding to the point data. Lock data not provided if there is no lock.
inline void savePoints(const HoverPoints::Profile & profile, QDomDocument & doc, QDomElement & points) {
    for (int a = 0; a < profile.points.size(); a++) {
        QPointF currPoint = profile.points.at(a);
        QDomElement currPointElem = doc.createElement(POINT_ELEMENT);
        if (profile.locks.at(a) != 0) {
            QDomAttr lockAttr = doc.createAttribute(LOCK_ATTR);
            lockAttr.setValue(QString::number(profile.locks.at(a)));
            currPointElem.setAttributeNode(lockAttr);
        }

        QDomAttr xAttr = doc.createAttribute(X_ATTR);
        xAttr.setValue(QString::number(currPoint.x()));
        currPointElem.setAttributeNode(xAttr);

        QDomAttr yAttr = doc.createAttribute(Y_ATTR);
        yAttr.setValue(QString::number(currPoint.y()));
        currPointElem.setAttributeNode(yAttr);

        points.appendChild(currPointElem);
    }
}

//! Given the profile and filename, save the profile at the filename
void ProfileHandler::saveProfile(const Profile & profile, const QString & filename) {
    QDomDocument doc(DOC_TYPE);
    QDomElement root = doc.createElement(DOC_TYPE);
    doc.appendChild(root);

    QDomAttr nameAttr = doc.createAttribute(NAME_ATTR);
    nameAttr.setValue(profile.name);
    root.setAttributeNode(nameAttr);

    QDomElement hoverPoints = doc.createElement(HOVERPOINTS_ELEMENT);
    root.appendChild(hoverPoints);

    QDomElement points = doc.createElement(POINTS_ELEMENT);
    savePoints(profile.hoverPointsProfile, doc, points);
    hoverPoints.appendChild(points);

    QDomElement dataSpace = doc.createElement(DATASPACE_ELEMENT);
    saveDataSpace(profile.hoverPointsProfile, doc, dataSpace);
    hoverPoints.appendChild(dataSpace);

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << doc.toString();
}


ProfileWidget::ProfileWidget(QWidget * parent, HoverPoints * hoverPoints) : QWidget(parent) {
    nextId = 0;
    idCache = 0;
    this->hoverPoints = hoverPoints;
    setupUi(this);
    connect(profileButtonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(resetProfiles()));
    connect(profileButtonBox->button(QDialogButtonBox::Open), SIGNAL(clicked()), this, SLOT(openProfiles()));
    connect(profileButtonBox->button(QDialogButtonBox::SaveAll), SIGNAL(clicked()), this, SLOT(saveAllProfiles()));
    connect(profileButtonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(saveProfile()));
    connect(deleteProfileButton, SIGNAL(clicked()), this, SLOT(deleteProfile()));
    connect(newProfileButton, SIGNAL(clicked()), this, SLOT(newProfile()));
    connect(profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(loadProfile(int)));;
    connect(profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(lineEditMatch()));

    returnPressed = false;
    connect(profileLineEdit, SIGNAL(returnPressed()), this, SLOT(setReturnPressed()));
    connect(profileLineEdit, SIGNAL(editingFinished()), this, SLOT(lineEditMatch()));
}

//! Load the starting profiles from cache
void ProfileWidget::loadDefaultProfiles() {
    QDir dir = QDir::current();
    QStringList fileList;
    if (dir.exists(CACHE_FOLDER)) {
        dir.cd(CACHE_FOLDER);

        QStringList nameFilter;
        nameFilter << QString("*").append(XML_EXT);
        fileList = dir.entryList(nameFilter);
    }

    if (fileList.size() > 0) {
        for (int a = 0; a < fileList.size(); a++) {
            Profile profile;
            QString filePath = QString(CACHE_FOLDER).append("/").append(fileList.at(a));
            if (ProfileHandler::loadProfile(profile, filePath))
                addProfile(profile, false);
            else
                cout << "Failed to open: " << qPrintable(filePath) << endl;
        }
    }
    else
        newProfile();
}

//! Saves the profile currenly on profileComboBox on the location provided by the dialog box
void ProfileWidget::saveProfile() {
    if (profileMap.contains(idCache)) {
        QString filename = QFileDialog::getSaveFileName(this, "Save profile", FILE_DIALOG_DIR, QString("XML files (*").append(XML_EXT).append(")"));

        //Append the xml extension is not provided
        if (!filename.endsWith(XML_EXT))
            filename.append(XML_EXT);

        //Update the current profile
        Profile & profile = profileMap.find(idCache).value();
        hoverPoints->getProfile(profile.hoverPointsProfile);

        //Save the profile
        ProfileHandler::saveProfile(profile, filename);
    }
    else if (profileMap.size() > 0) {
        cerr << "ProfileWidget.cpp: Error, idCache not valid." << endl;
    }
}

//! Given a file directory, save all profiles. Will append a number to the filename if the file exists
void ProfileWidget::saveAllProfilesAt(const QString & dir) {
    saveCacheProfile();

    QList<Profile> profiles = profileMap.values();

    for (int a = 0; a < profiles.size(); a++) {
        uint num = 1;
        QString fileDir = dir;
        fileDir.append("/").append(profiles.at(a).name);

        QString tempFileDir = fileDir;
        tempFileDir.append(XML_EXT);
        for (;QFile::exists(tempFileDir);)
            tempFileDir = QString(fileDir).append(QString::number(num++)).append(XML_EXT);

        ProfileHandler::saveProfile(profiles.at(a), tempFileDir);
    }
}

void ProfileWidget::saveAllProfiles() {
    saveAllProfilesAt(QFileDialog::getExistingDirectory(this, "Directory to save profile(s)", FILE_DIALOG_DIR));
}

void ProfileWidget::openProfiles() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Load profile(s)", FILE_DIALOG_DIR, QString("XML files (*").append(XML_EXT).append(")"));

    for (int a = 0; a < files.size(); a++) {
        Profile profile;
        if (ProfileHandler::loadProfile(profile, files.at(a)))
            addProfile(profile);
        else
            cout << "Failed to open: " << qPrintable(files.at(a)) << endl;
    }
}

//! Makes a new default profile, adds the profile and loads it
void ProfileWidget::newProfile() {
    if (profileMap.size() == 0)
        idCache = nextId;

    Profile profile;
    emit getProfileSignal(profile);
    profile.name = NEW_PROFILE_NAME;
    addProfile(profile);
}

void ProfileWidget::deleteProfile() {
    removeProfile(profileComboBox->currentIndex());
}

//! Loads the profile and ensures that the id cache is correct
void ProfileWidget::loadProfile(int index) {
    if (index != -1) {
        saveCacheProfile();

        uint id = profileComboBox->itemData(index).toUInt();
        idCache = id;
        if (profileMap.contains(id)) {
            const Profile & profile = profileMap.find(id).value();
            hoverPoints->loadProfile(profile.hoverPointsProfile);
        }
        else {
            cerr << "ProfileWidget.cpp: Error, mapping between profileMap and profileComboBox not matched." << endl;
        }
    }
}

//! Resets the profiles back to the original state. Prompts the user before acting
void ProfileWidget::resetProfiles() {
    QMessageBox messageBox;
    messageBox.setText(RESET_MESSAGE);
    messageBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    messageBox.setDefaultButton(QMessageBox::Cancel);
    int ret = messageBox.exec();

    if (ret == QMessageBox::Ok) {
        profileMap.clear();
        profileComboBox->clear();
        newProfile();
    }
}

//! Helps detect whethter the return key is pressed
void ProfileWidget::setReturnPressed() {
    returnPressed = true;
}

//! Line edit will change the profile name if the return key is pressed, otherwise the line edit will reset
void ProfileWidget::lineEditMatch() {
    if (!profileMap.empty()) {
        if (returnPressed) {
            profileComboBox->setItemText(profileComboBox->currentIndex(), profileLineEdit->text());
            profileMap.find(idCache).value().name = profileLineEdit->text();
            returnPressed = false;
        }
        else {
            const QString & text = profileMap.find(idCache).value().name;
            profileLineEdit->setText(text);
        }
    }
}

//! Saves the last profile state into the right profile
void ProfileWidget::saveCacheProfile() {
    if (nextId != idCache) {
        if (profileMap.contains(idCache)) {
            Profile & profile = profileMap.find(idCache).value();
            hoverPoints->getProfile(profile.hoverPointsProfile);
        }
        else if (profileMap.size() > 0) {
            cerr << "ProfileWidget.cpp: Error, idCache not valid." << endl;
        }
    }
}

//! Adds profile into profileComboxBox. If select = true, load the provided profile
void ProfileWidget::addProfile(const Profile & profile, bool select) {
    profileMap.insert(nextId, profile);
    profileComboBox->addItem(profile.name, nextId);
    nextId++;
    if (select) {
        loadProfile(profileComboBox->count() - 1);
        profileComboBox->setCurrentIndex(profileComboBox->count() -1);
    }
}

//! Removes the profile at index, and makes a new profile if profileComboBox is empty
void ProfileWidget::removeProfile(int index) {
    uint id = profileComboBox->itemData(index).toUInt();
    profileComboBox->removeItem(index);
    profileMap.remove(id);
    if (profileComboBox->count() == 0) {
        newProfile();
    }
}


//! Saves all of the current profiles into the cache folder when the widget closes
void ProfileWidget::hideEvent(QHideEvent *) {
    QDir dir = QDir::current();
    if (dir.exists(CACHE_FOLDER)) {
        dir.cd(CACHE_FOLDER);
        QStringList nameFilter;
        nameFilter << QString("*").append(XML_EXT);
        QStringList fileList = dir.entryList(nameFilter);
        for (int a = 0; a < fileList.size(); a++)
            dir.remove(fileList.at(a));
    }
    else {
        dir.mkdir(CACHE_FOLDER);
        dir.cd(CACHE_FOLDER);
    }
    saveAllProfilesAt(dir.absolutePath());
}
