#include "settingsmanager.h"
#include <QDebug>

SettingsManager::SettingsManager()
    : settings(QSettings::IniFormat, QSettings::UserScope, "GDP1", "RTK_VisPos")
{
    loadSettings();
}


void SettingsManager::loadSettings(){
    qDebug() << "Loading Settings...";

    m_guiSettings.setTheme(static_cast<themes_t>(settings.value("GUI/theme", AQUA).toInt()));

    m_rendererSettings.setIsSeparateWindow(settings.value("Renderer/isSeparateWindow", false).toBool());

}

void SettingsManager::saveSettings() {
    qDebug() << "Saving settings...";
    qDebug() << "Settings file path: " << settings.fileName();



    settings.setValue("GUI/theme", static_cast<int>(m_guiSettings.getTheme()));


    settings.setValue("Renderer/isSeparateWindow", m_rendererSettings.getIsSeparateWindow());
}


GUISettings& SettingsManager::guiSettings() {
    return m_guiSettings;
}

const GUISettings& SettingsManager::guiSettings() const {
    return m_guiSettings;
}

RendererSettings& SettingsManager::rendererSettings() {
    return m_rendererSettings;
}

const RendererSettings& SettingsManager::rendererSettings() const {
    return m_rendererSettings;
}


