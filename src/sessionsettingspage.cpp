#include "sessionsettingspage.h"

SessionSettingsPage::SessionSettingsPage(QWidget *parent) : QWidget(parent) {

    QVBoxLayout *sessionManagementLayout = new QVBoxLayout(this);
    QLabel *sessionLabel = new QLabel("Session Management Settings", this);
    sessionManagementLayout->addWidget(sessionLabel);
    setLayout(sessionManagementLayout);
}
