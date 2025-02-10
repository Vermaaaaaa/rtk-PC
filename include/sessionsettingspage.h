#ifndef SESSIONSETTINGSPAGE_H
#define SESSIONSETTINGSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class SessionSettingsPage : public QWidget
{
    Q_OBJECT
public:
   explicit SessionSettingsPage(QWidget *parent = nullptr);
};

#endif // SESSIONSETTINGSPAGE_H
