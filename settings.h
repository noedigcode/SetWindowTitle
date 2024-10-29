#ifndef SETTINGS_H
#define SETTINGS_H

#include "gidfile.h"

#include <QJsonObject>
#include <QSharedPointer>
#include <QString>

class Settings
{
public:
    Settings();

    const QString settingsDirName {"SetWindowTitle-settings"};

    // -------------------------------------------------------------------------

    struct Preset
    {
        QString regexPattern;
        QString windowTitle;

        QJsonObject toJson();
        void fromJson(QJsonObject json);
    };
    typedef QSharedPointer<Preset> PresetPtr;

    // -------------------------------------------------------------------------
    // Settings

    QList<PresetPtr> presets;

    // -------------------------------------------------------------------------

    QJsonObject toJson();
    void fromJson(QJsonObject json);

    QString settingsFilePath();
    QString settingsDir();

    GidFile::Result save();
    GidFile::Result load();
};

#endif // SETTINGS_H
