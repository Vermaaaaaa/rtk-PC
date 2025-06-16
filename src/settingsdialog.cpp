#include "settingsdialog.h"
#include "sessionsettingspage.h"
#include "AnimationPage.h"
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
    AnimationPage *animationPage = new AnimationPage(this);
    ThemeSettingsPage *themeSettingsPage = new ThemeSettingsPage(settings, this);

    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);

    /*
    connect(applyButton, &QPushButton::clicked, this, [this, rendererSettingsPage, themeSettingsPage]() {
        applyChanges(rendererSettingsPage, themeSettingsPage);
    });
*/

    connect(sessionSettingsPage, &SessionSettingsPage::generatePlotsRequested,
            this, &SettingsDialog::generatePlotsRequested);

    connect(animationPage, &AnimationPage::requestAnimationFile, this, &SettingsDialog::requestAnimationFile);
    connect(animationPage, &AnimationPage::generateAnimationRequested, this, &SettingsDialog::generateAnimationRequested);

    connect(animationPage,&AnimationPage::generateAnimationRequested,this, &SettingsDialog::hide);



    buttonLayout->setAlignment(Qt::AlignBottom);


    listWidget = new QListWidget(this);

    listWidget->addItem("Plot");
    listWidget->addItem("Animation");
    listWidget->addItem("Themes");



    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(sessionSettingsPage);
    stackedWidget->addWidget(animationPage);
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

void SettingsDialog::applyChanges(ThemeSettingsPage *themeSettingsPage){
    themes_t theme = themeSettingsPage->get_selectedTheme();
    settings->guiSettings().setTheme(theme);
    static_cast<MainWindow*>(parent())->applyStyleSheet(theme);




}

