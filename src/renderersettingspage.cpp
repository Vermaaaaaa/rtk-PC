#include "renderersettingspage.h"

RendererSettingsPage::RendererSettingsPage(SettingsManager *settingsManager, QWidget *parent) : QWidget(parent), m_settingsManager(settingsManager) {
    QVBoxLayout *rendererLayout = new QVBoxLayout(this);
    QHBoxLayout *renderToSepWindowLayout = new QHBoxLayout();

    QLabel *rendererLabel = new QLabel("Render to a separate window", this);
    QCheckBox *rendererCheckBox = new QCheckBox(this);

    renderToSepWindowLayout->addWidget(rendererLabel);
    renderToSepWindowLayout->addWidget(rendererCheckBox);

    renderToSepWindowLayout->setAlignment(rendererLabel, Qt::AlignTop);
    renderToSepWindowLayout->setAlignment(rendererCheckBox, Qt::AlignTop);

    rendererLayout->addLayout(renderToSepWindowLayout);
    setLayout(rendererLayout);

    //bool isSepWin = m_settingsManager->rendererSettings().getIsSeparateWindow();

}
