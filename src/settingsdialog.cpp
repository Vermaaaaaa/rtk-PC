#include "settingsdialog.h"
#include "renderersettingspage.h"
#include "sessionsettingspage.h"
#include "themesettingspage.h"
#include "settingsmanager.h"
#include "mainwindow.h"

#include <QPushButton>

SettingsDialog::SettingsDialog(SettingsManager *Settings, QWidget *parent) : QDialog(parent), settings(Settings) {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    applyButton = new QPushButton("Apply", this);
    QPushButton *cancelButton = new QPushButton("Cancel",this);

    SessionSettingsPage *sessionSettingsPage = new SessionSettingsPage(this);
    RendererSettingsPage *rendererSettingsPage = new RendererSettingsPage(settings, this);
    ThemeSettingsPage *themeSettingsPage = new ThemeSettingsPage(settings, this);

    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);

    connect(applyButton, &QPushButton::clicked, this, [this, rendererSettingsPage, themeSettingsPage]() {
        applyChanges(rendererSettingsPage, themeSettingsPage);
    });


    buttonLayout->setAlignment(Qt::AlignBottom);


    listWidget = new QListWidget(this);

    listWidget->addItem("Session");
    listWidget->addItem("3D Renderer");
    listWidget->addItem("Themes");



    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(sessionSettingsPage);
    stackedWidget->addWidget(rendererSettingsPage);
    stackedWidget->addWidget(themeSettingsPage);



    mainLayout->addWidget(listWidget);
    mainLayout->addWidget(stackedWidget);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    connect(listWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
}

int SettingsDialog::getCurrentRow(){
    return this->currentRow;
}

void SettingsDialog::setCurrentRow(int row){
    this->currentRow = row;
    listWidget->setCurrentRow(row);
}

void SettingsDialog::applyChanges(RendererSettingsPage *rendererSettingsPage, ThemeSettingsPage *themeSettingsPage){
    themes_t theme = themeSettingsPage->get_selectedTheme();
    settings->guiSettings().setTheme(theme);

    QString themePath;
    switch (theme) {
    case MATERIAL_DARK:
        themePath = ":/MaterialDark.qss";
        break;
    case MANJARO_MIX:
        themePath = ":/ManjaroMix.qss";
        break;
    case AMOLED:
        themePath = ":/AMOLED.qss";
        break;
    case AQUA:
        themePath = ":/Aqua.qss";
        break;
    case CONSOLE_STYLE:
        themePath = ":/ConsoleStyle.qss";
        break;
    case ELEGANT_DARK:
        themePath = ":/ElegantDark.qss";
        break;
    case UBUNTU:
        themePath = ":/Ubuntu.qss";
        break;
    }

    static_cast<MainWindow*>(parent())->applyStyleSheet(themePath);

}

