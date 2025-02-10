#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "guisettings.h"
#include "renderersettings.h"
#include <QSettings>


class SettingsManager
{
public:
    SettingsManager();

    void loadSettings();
    void saveSettings();


    const GUISettings& guiSettings() const;
    GUISettings& guiSettings();

    const RendererSettings& rendererSettings() const;
    RendererSettings& rendererSettings();

private:
    GUISettings m_guiSettings;
    RendererSettings m_rendererSettings;

    QSettings settings;
};

#endif // SETTINGSMANAGER_H
