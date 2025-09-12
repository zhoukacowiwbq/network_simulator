#include "HeatmapDialog.h"  

// QtChart  
#include <QtCharts/QChartView>  
#include <QtCharts/QChart>  
#include <QtCharts/QScatterSeries>  
#include <QtCharts/QValueAxis>  

#include <QTimer>  
#include <QVBoxLayout>  
#include <QRandomGenerator>  


HeatmapDialog::HeatmapDialog(QWidget* parent)  
    : QWidget(parent)  
    , m_chart(new QChart())  
    , m_chartView(new QChartView(m_chart, this))  
    , m_scatterSeries(new QScatterSeries(this))  
    , m_axisX(new QValueAxis(this))  
    , m_axisY(new QValueAxis(this))  
    , m_timer(new QTimer(this))  
{  
    // 基础配置  
    m_chart->setTitle(QStringLiteral("热力图示例"));  
    m_chart->setAnimationOptions(QChart::AllAnimations);  

    m_scatterSeries->setName("Heatmap");  
    m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeRectangle);  
    m_scatterSeries->setMarkerSize(15);  

    m_chart->addSeries(m_scatterSeries);  

    // X 轴  
    m_axisX->setRange(0, 9);  
    m_axisX->setTickCount(10);  
    m_axisX->setTitleText(QStringLiteral("X 方向"));  
    m_chart->addAxis(m_axisX, Qt::AlignBottom);  
    m_scatterSeries->attachAxis(m_axisX);  

    // Y 轴  
    m_axisY->setRange(0, 9);  
    m_axisY->setTickCount(10);  
    m_axisY->setTitleText(QStringLiteral("Y 方向"));  
    m_chart->addAxis(m_axisY, Qt::AlignLeft);  
    m_scatterSeries->attachAxis(m_axisY);  

    // 布局  
    m_chartView->setRenderHint(QPainter::Antialiasing);  
    QVBoxLayout* layout = new QVBoxLayout(this);  
    layout->addWidget(m_chartView);  
    setLayout(layout);  

    // 定时更新随机热力数据  
    connect(m_timer, &QTimer::timeout, this, &HeatmapDialog::onUpdateHeatmap);  
    m_timer->start(1500); // 每1.5秒更新一次  
}  

HeatmapDialog::~HeatmapDialog()  
{  
}  

void HeatmapDialog::onUpdateHeatmap()  
{  
    m_scatterSeries->clear();  
    // 模拟 30 个随机点，实际可替换为二维分布数据  
    for (int i = 0; i < 30; ++i) {  
        qreal x = QRandomGenerator::global()->bounded(10);  
        qreal y = QRandomGenerator::global()->bounded(10);  
        m_scatterSeries->append(x, y);  
    }  
}  