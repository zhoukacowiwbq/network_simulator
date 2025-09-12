#include "BarChartDialog.h"  

// QtChart  
#include <QtCharts/QChartView>  
#include <QtCharts/QChart>  
#include <QtCharts/QBarSeries>  
#include <QtCharts/QBarSet>  
#include <QtCharts/QCategoryAxis>  
#include <QtCharts/QValueAxis>  
#include <QtCharts/QBarCategoryAxis>  

#include <QTimer>  
#include <QVBoxLayout>  
#include <QRandomGenerator>  


BarChartDialog::BarChartDialog(QWidget* parent)  
    : QWidget(parent)  
    , m_chart(new QChart())  
    , m_chartView(new QChartView(m_chart, this))  
    , m_barSeries(new QBarSeries(this))  
    , m_xAxis(new QBarCategoryAxis(this))  
    , m_yAxis(new QValueAxis(this))  
    , m_timer(new QTimer(this))  
{  

    // 1. 设定类别  
    m_categories << "Group 1" << "Group 2" << "Group 3";  

    // 2. 每一 BarSet 代表一个系列组  
    // 示例添加两个 BarSet 模拟  
    QBarSet* setA = new QBarSet("Set A");  
    QBarSet* setB = new QBarSet("Set B");  

    // 初始化随机值  
    for (int i = 0; i < m_categories.size(); ++i) {  
        *setA << QRandomGenerator::global()->bounded(100);  
        *setB << QRandomGenerator::global()->bounded(100);  
    }  

    // 自定义颜色  
    setA->setColor(Qt::magenta);  
    setB->setColor(Qt::cyan);  

    m_barSets.append(setA);  
    m_barSets.append(setB);  

    m_barSeries->append(setA);  
    m_barSeries->append(setB);  

    m_chart->addSeries(m_barSeries);  
    m_chart->setTitle(QStringLiteral("柱状图演示"));  

    // X 轴  
    m_xAxis->append(m_categories);  
    m_chart->addAxis(m_xAxis, Qt::AlignBottom);  
    m_barSeries->attachAxis(m_xAxis);  

    // Y 轴  
    m_yAxis->setRange(0, 100);  
    m_yAxis->setTitleText(QStringLiteral("数值"));  
    m_chart->addAxis(m_yAxis, Qt::AlignLeft);  
    m_barSeries->attachAxis(m_yAxis);  

    // 视图布局  
    m_chartView->setRenderHint(QPainter::Antialiasing);  
    QVBoxLayout* layout = new QVBoxLayout(this);  
    layout->addWidget(m_chartView);  
    setLayout(layout);  

    // 定时更新  
    connect(m_timer, &QTimer::timeout, this, &BarChartDialog::onUpdateBarChart);  
    m_timer->start(2000); // 每2秒刷新  
}  

BarChartDialog::~BarChartDialog()  
{  
    // 自动析构  
}  

void BarChartDialog::onUpdateBarChart()  
{  
    // 更新随机值，也可替换为真实数据  
    for (auto barSet : m_barSets) {  
        for (int i = 0; i < barSet->count(); ++i) {  
            barSet->replace(i, QRandomGenerator::global()->bounded(100));  
        }  
    }  
}  