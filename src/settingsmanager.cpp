#include "settingsmanager.h"
#include <QDebug>

SettingsManager::SettingsManager()
    : settings(QSettings::IniFormat, QSettings::UserScope, "GDP1", "RTK_VisPos")  // Change "MyCompany" and "MyApp" to your application's name
{
    loadSettings();  // Load settings when the manager is created
}


void SettingsManager::loadSettings(){
    qDebug() << "Loading Settings...";

    // Load GUI settings
    m_guiSettings.setTheme(static_cast<themes_t>(settings.value("GUI/theme", AQUA).toInt()));

    // Load Renderer settings
    m_rendererSettings.setIsSeparateWindow(settings.value("Renderer/isSeparateWindow", false).toBool());

}

void SettingsManager::saveSettings() {
    qDebug() << "Saving settings...";
    qDebug() << "Settings file path: " << settings.fileName();


    // Save GUI settings
    settings.setValue("GUI/theme", static_cast<int>(m_guiSettings.getTheme()));

    // Save Renderer settings
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


