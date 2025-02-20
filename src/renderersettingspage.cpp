#include "renderersettingspage.h"

RendererSettingsPage::RendererSettingsPage(SettingsManager *settingsManager, QWidget *parent) : QWidget(parent), m_settingsManager(settingsManager) {
    QVBoxLayout *rendererLayout = new QVBoxLayout(this);
    QHBoxLayout *renderToSepWindowLayout = new QHBoxLayout();

    rendererCheckBox = new QCheckBox("Render to a separate window",this);

    renderToSepWindowLayout->addWidget(rendererCheckBox);

    renderToSepWindowLayout->setAlignment(rendererCheckBox, Qt::AlignTop);

    rendererLayout->addLayout(renderToSepWindowLayout);
    setLayout(rendererLayout);

    bool isSepWin = m_settingsManager->rendererSettings().getIsSeparateWindow();

    rendererCheckBox->setChecked(isSepWin);

}

QCheckBox* RendererSettingsPage::getRendererCheckBox(){
    return rendererCheckBox;
}
