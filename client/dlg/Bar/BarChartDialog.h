#ifndef BARCHARTDIALOG_H  
#define BARCHARTDIALOG_H  

#include <QWidget>  
#include <QList>  
#include <QStringList>  

class QChart;  
class QChartView;  
class QTimer;  
class QBarSeries;  
class QBarCategoryAxis;  
class QValueAxis;  
class QBarSet;  

class BarChartDialog : public QWidget  
{  
    Q_OBJECT  
public:  
    explicit BarChartDialog(QWidget* parent = nullptr);  
    ~BarChartDialog();  

private slots:  
    void onUpdateBarChart();  

private:  
    QChart*             m_chart;        // 图表  
    QChartView*         m_chartView;    // 图表视图  
    QBarSeries*         m_barSeries;    // 柱状数据系列  
    QBarCategoryAxis*   m_xAxis;        // X 轴 (类别轴)  
    QValueAxis*         m_yAxis;        // Y 轴 (数值轴)  
    QStringList         m_categories;   // 类别名称  
    QList<QBarSet*>     m_barSets;      // 每个柱状组  
    QTimer*             m_timer;        // 定时器  
};  

#endif // BARCHARTDIALOG_H  