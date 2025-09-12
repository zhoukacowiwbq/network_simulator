#pragma once  

#include <QWidget>  
#include <QTableWidget>  
#include <QVBoxLayout>  
#include <QTimer>  
#include <QStringList>  
#include <QRandomGenerator>  

class TrafficTableDialog : public QWidget  
{  
    Q_OBJECT  

public:  
    explicit TrafficTableDialog(QWidget* parent = nullptr);  
    ~TrafficTableDialog();  

private slots:  
    void onUpdateTableData();  // 定时刷新表格数据  

private:  
    QTableWidget* m_tableWidget;  // 表格控件  
    QTimer*       m_timer;        // 定时器  
    QStringList   m_interfaces;   // 模拟的接口名称列表  
};  