#ifndef ANIMATIONPAGE_H
#define ANIMATIONPAGE_H


#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QStringList>


class AnimationPage : public QWidget
{
    Q_OBJECT
public:
    explicit AnimationPage(QWidget *parent = nullptr);

signals:
    void generateAnimationRequested();
    void requestAnimationFile(const QString &filePath);
private slots:
    void onOpenSessionClicked();
    void onGenerateClicked();

private:
    QPushButton *openButton;
    QLabel      *fileLabel;
    QPushButton *generateButton;
    QString      sessionFilePath;

    QStringList   m_diffLines;
    int           m_diffIndex = 0;

    void setControlsEnabled(bool on);

};

#endif // ANIMATIONPAGE_H
