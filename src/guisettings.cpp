#include "guisettings.h"

void GUISettings::setTheme(const themes_t theme){
    m_theme = theme;
}

themes_t GUISettings::getTheme() const{
    return m_theme;
}
