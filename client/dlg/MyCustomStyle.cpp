#include "MyCustomStyle.h"  
#include <QColor>  

QPalette MyCustomStyle::standardPalette() const  
{  
    QPalette darkPalette = QProxyStyle::standardPalette();  

    // 统一采用暗色系  
    darkPalette.setColor(QPalette::Window, QColor(30, 31, 43));  
    darkPalette.setColor(QPalette::WindowText, QColor(220, 220, 220));  
    darkPalette.setColor(QPalette::Base, QColor(20, 20, 30));  
    darkPalette.setColor(QPalette::AlternateBase, QColor(40, 40, 50));  
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);  
    darkPalette.setColor(QPalette::ToolTipText, Qt::black);  
    darkPalette.setColor(QPalette::Text, QColor(220, 220, 220));  
    darkPalette.setColor(QPalette::Button, QColor(60, 61, 80));  
    darkPalette.setColor(QPalette::ButtonText, QColor(240, 240, 240));  
    darkPalette.setColor(QPalette::BrightText, Qt::red);  
    darkPalette.setColor(QPalette::Highlight, QColor(80, 81, 100));  
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);  

    return darkPalette;  
}  