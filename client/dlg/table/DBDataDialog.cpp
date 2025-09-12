#include "DBDataDialog.h"
#include <QHeaderView>
#include <qdatetime.h>

TrafficTableDialog::TrafficTableDialog(QWidget* parent)  
    : QWidget(parent)  
    , m_tableWidget(new QTableWidget(this))  
    , m_timer(new QTimer(this))  
    , m_interfaces({"eth0", "eth1", "eth2", "eth3", "eth4", "eth5", "eth6"})  // 模拟接口名  
{  

    QVBoxLayout* layout = new QVBoxLayout(this);  
    layout->addWidget(m_tableWidget);  
    setLayout(layout);  

    // 初始化表格  
    m_tableWidget->setColumnCount(5);  // 5 列  
    m_tableWidget->setRowCount(m_interfaces.size());  

    QStringList headers = {"接口名称", "上行流量 (Mbps)", "下行流量 (Mbps)", "丢包率 (%)", "更新时间"};  
    m_tableWidget->setHorizontalHeaderLabels(headers);  
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);  // 最后一列拉伸  

    for (int i = 0; i < m_interfaces.size(); ++i) {  
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(m_interfaces.at(i))); // 接口名称  
    }  

    // 定时器刷新表格内容  
    connect(m_timer, &QTimer::timeout, this, &TrafficTableDialog::onUpdateTableData);  
    m_timer->start(1500);  // 每 1.5 秒刷新一次  
}  

TrafficTableDialog::~TrafficTableDialog()  
{  
    // 无需手动清理  
}  

void TrafficTableDialog::onUpdateTableData()  
{  
    for (int row = 0; row < m_interfaces.size(); ++row) {  
        // 随机生成上行流量  
        double upTraffic = QRandomGenerator::global()->bounded(100);  
        m_tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(upTraffic, 'f', 2)));  

        // 随机生成下行流量  
        double downTraffic = QRandomGenerator::global()->bounded(100);  
        m_tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(downTraffic, 'f', 2)));  

        // 随机生成丢包率  
        double lossRate = QRandomGenerator::global()->bounded(10); // 随机 0 ~ 10%  
        m_tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(lossRate, 'f', 2)));  

        // 更新时间戳  
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");  
        m_tableWidget->setItem(row, 4, new QTableWidgetItem(timestamp));  
    }  
}  