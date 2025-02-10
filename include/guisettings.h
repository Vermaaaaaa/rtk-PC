#ifndef GUISETTINGS_H
#define GUISETTINGS_H

typedef enum{
    MATERIAL_DARK,
    MANJARO_MIX,
    AMOLED,
    AQUA,
    CONSOLE_STYLE,
    ELEGANT_DARK,
    UBUNTU
}themes_t;

class GUISettings{

public:
    themes_t getTheme() const;
    void setTheme(themes_t theme);

private:
    themes_t m_theme = AQUA;
};

#endif // GUISETTINGS_H
