#include "AnimationPage.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QGroupBox>


AnimationPage::AnimationPage(QWidget *parent)
    : QWidget(parent)
{
    auto *lay = new QVBoxLayout(this);


    openButton = new QPushButton(tr("Open Session…"), this);
    fileLabel  = new QLabel(tr("<no file selected>"), this);
    lay->addWidget(openButton);
    lay->addWidget(fileLabel);



    generateButton = new QPushButton(tr("Generate Animation"), this);
    lay->addWidget(generateButton);


    connect(openButton,    &QPushButton::clicked, this, &AnimationPage::onOpenSessionClicked);
    connect(generateButton,&QPushButton::clicked, this, &AnimationPage::onGenerateClicked);
}



void AnimationPage::onOpenSessionClicked() {
    QString filter = tr("CSV Files (*.csv)");
    QString fn = QFileDialog::getOpenFileName(this,
                                              tr("Select Session File"), QString(), filter);
    if (fn.isEmpty()) return;

    sessionFilePath = fn;
    fileLabel->setText(fn);

    emit requestAnimationFile(sessionFilePath);

}

void AnimationPage::onGenerateClicked() {
    if (sessionFilePath.isEmpty()) return;

    emit generateAnimationRequested();

}
