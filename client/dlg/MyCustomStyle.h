#ifndef MYCUSTOMSTYLE_H  
#define MYCUSTOMSTYLE_H  

#include <QProxyStyle>  
#include <QPalette>  

class MyCustomStyle : public QProxyStyle  
{  
    Q_OBJECT  
public:  
    using QProxyStyle::QProxyStyle;  

    // 重写标准调色板，实现暗色系  
    QPalette standardPalette() const override;  
};  

#endif // MYCUSTOMSTYLE_H  