#ifndef THEMESETTINGSPAGE_H
#define THEMESETTINGSPAGE_H

#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QObject>
#include "settingsmanager.h"

class ThemeSettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeSettingsPage(SettingsManager *settingsManager, QWidget *parent = nullptr);
    themes_t get_selectedTheme() const;

signals:

private:
    QComboBox *themeComboBox;
    const SettingsManager *m_settingsManager;
};

#endif // THEMESETTINGSPAGE_H
