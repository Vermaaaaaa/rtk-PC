#ifndef RENDERERSETTINGSPAGE_H
#define RENDERERSETTINGSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "settingsmanager.h"
#include <QCheckbox>

class RendererSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit RendererSettingsPage(SettingsManager *settingsManager, QWidget *parent = nullptr);
    QCheckBox* getRendererCheckBox();

private:
    const SettingsManager *m_settingsManager;
    QCheckBox *rendererCheckBox;
};

#endif // RENDERERSETTINGSPAGE_H
