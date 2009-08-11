#ifndef PROFILE_WIDGET_H
#define PROFILE_WIDGET_H

#include "ui_ProfileWidget.h"
#include "HoverPoints.h"

#define XML_EXT ".xml"
#define FILE_DIALOG_DIR "."
#define CACHE_FOLDER "cache"
#define RESET_MESSAGE "Reset all profiles? This will delete all unsaved profiles."
#define NEW_PROFILE_NAME "new profile"

//! Namespace that handles saving and loading profiles
namespace ProfileHandler {
    //! Struct of what a profile is comrprised of
    struct Profile {
        QString name;
        HoverPoints::Profile hoverPointsProfile;
    };

    bool loadProfile(Profile & profile, const QString & filename);
    void saveProfile(const Profile & profile, const QString & filename);
}

/*!
    Widget that handles user interaction with profiles. The widget should be fairly inuitive,
    other than the line edit, which modifies the profile name when the return key is pressed.
 */
class ProfileWidget : public QWidget, protected Ui::ProfileWidget {
    Q_OBJECT
    //! Map between the id and the corresponding profile, profileComboBox items have ids that link to this
    QHash<uint, ProfileHandler::Profile> profileMap;
    //! Pointer to the hoverpoints used in the GraphWidget
    HoverPoints * hoverPoints;
    //! Next id to be used
    uint nextId;
    //! Cache of the current id to save the profile state if the provide is changed in profileComboBox
    uint idCache;
    //! true if return is pressed in profileLineEdit
    bool returnPressed;

    void saveCacheProfile();
    void addProfile(const ProfileHandler::Profile & profile, bool select = true);
    void removeProfile(int index);
    void saveAllProfilesAt(const QString & dir);
public:
    ProfileWidget(QWidget * parent = NULL, HoverPoints * hoverPoints = NULL);
    void loadDefaultProfiles();

protected:
    virtual void hideEvent(QHideEvent * event);
    
private slots:
    void saveProfile();
    void saveAllProfiles();
    void openProfiles();
    void newProfile();
    void loadProfile(int index);
    void deleteProfile();
    void resetProfiles();

    void setReturnPressed();
    void lineEditMatch();
signals:
    void getProfileSignal(ProfileHandler::Profile & profile);
    void loadedProfile(ProfileHandler::Profile & profile);
};

#endif // Profile_WIDGET_H
