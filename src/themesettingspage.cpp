#include "themesettingspage.h"
#include "guisettings.h"


ThemeSettingsPage::ThemeSettingsPage(SettingsManager *settingsManager, QWidget *parent) : QWidget(parent) , m_settingsManager(settingsManager){
    QVBoxLayout *themeLayout = new QVBoxLayout(this);
    QHBoxLayout *selectorLayout = new QHBoxLayout();


    QLabel *themeLabel = new QLabel("Themes", this);
    themeComboBox = new QComboBox(this);

    themeComboBox->addItem("AMOLED", QVariant::fromValue(themes_t::AMOLED));
    themeComboBox->addItem("Aqua", QVariant::fromValue(themes_t::AQUA));
    themeComboBox->addItem("Console Style", QVariant::fromValue(themes_t::CONSOLE_STYLE));
    themeComboBox->addItem("Elegant Dark", QVariant::fromValue(themes_t::ELEGANT_DARK));
    themeComboBox->addItem("Manjaro Mix", QVariant::fromValue(themes_t::MANJARO_MIX));
    themeComboBox->addItem("Material Dark", QVariant::fromValue(themes_t::MATERIAL_DARK));
    themeComboBox->addItem("Ubuntu", QVariant::fromValue(themes_t::UBUNTU));

    selectorLayout->addWidget(themeLabel);
    selectorLayout->addWidget(themeComboBox);


    selectorLayout->setAlignment(themeLabel, Qt::AlignTop);
    selectorLayout->setAlignment(themeComboBox, Qt::AlignTop);

    themeLayout->addLayout(selectorLayout);
    setLayout(themeLayout);

    themes_t savedTheme = m_settingsManager->guiSettings().getTheme();
    int index = themeComboBox->findData(QVariant::fromValue(savedTheme));
    if (index >= 0) {
        themeComboBox->setCurrentIndex(index);
    }

}

themes_t ThemeSettingsPage::get_selectedTheme() const {
    return themeComboBox->currentData().value<themes_t>();
}



