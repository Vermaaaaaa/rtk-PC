#ifndef RENDERERSETTINGS_H
#define RENDERERSETTINGS_H

class RendererSettings
{
public:
    bool getIsSeparateWindow() const;
    void setIsSeparateWindow(bool sepWin);

private:
    bool m_isSeparateWindow;
};

#endif // RENDERERSETTINGS_H
