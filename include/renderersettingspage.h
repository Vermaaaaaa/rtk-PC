#ifndef RENDERERSETTINGSPAGE_H
#define RENDERERSETTINGSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "settingsmanager.h"

class RendererSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit RendererSettingsPage(SettingsManager *settingsManager, QWidget *parent = nullptr);

private:
    const SettingsManager *m_settingsManager;
};

#endif // RENDERERSETTINGSPAGE_H
