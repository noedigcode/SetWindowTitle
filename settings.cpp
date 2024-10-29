#include "settings.h"

#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>

Settings::Settings() {}

QJsonObject Settings::toJson()
{
    // Repos array
    QJsonArray aPresets;
    foreach (PresetPtr preset, presets) {
        aPresets.append(preset->toJson());
    }

    // Main settings object
    QJsonObject jMain;
    jMain.insert("presets", aPresets);

    return jMain;
}

void Settings::fromJson(QJsonObject json)
{
    QJsonArray aPresets = json.value("presets").toArray();
    foreach (QJsonValue v, aPresets) {
        QJsonObject obj = v.toObject();
        PresetPtr preset(new Preset());
        preset->fromJson(obj);
        presets.append(preset);
    }
}

QString Settings::settingsFilePath()
{
    return QString("%1/%2").arg(settingsDir()).arg(settingsDirName);
}

QString Settings::settingsDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

GidFile::Result Settings::save()
{
    QDir dir(settingsDir());
    if (!dir.exists()) {
        if (dir.mkpath(dir.path())) {
            qDebug() << "Created settings directory: " + dir.path();
        } else {
            qDebug() << "Failed to create settings directory: " + dir.path();
        }
    }

    QJsonObject jMain = toJson();

    QJsonDocument jDoc;
    jDoc.setObject(jMain);

    GidFile::Result r = GidFile::write(settingsFilePath(), jDoc.toJson());

    return r;
}

GidFile::Result Settings::load()
{
    GidFile::ReadResult r = GidFile::read(settingsFilePath());

    if (r.result.success) {
        QJsonDocument jDoc = QJsonDocument::fromJson(r.data);
        QJsonObject jMain = jDoc.object();
        fromJson(jMain);
    }

    return r.result;
}

QJsonObject Settings::Preset::toJson()
{
    QJsonObject j;
    j.insert("regexPattern", regexPattern);
    j.insert("windowTitle", windowTitle);
    return j;
}

void Settings::Preset::fromJson(QJsonObject json)
{
    regexPattern = json.value("regexPattern").toString();
    windowTitle = json.value("windowTitle").toString();
}
