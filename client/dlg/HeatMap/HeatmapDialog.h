#ifndef HEATMAPDIALOG_H  
#define HEATMAPDIALOG_H  

#include <QWidget>  

class QChart;  
class QChartView;  
class QScatterSeries;  
class QTimer;  
class QValueAxis;  

class HeatmapDialog : public QWidget  
{  
    Q_OBJECT  
public:  
    explicit HeatmapDialog(QWidget* parent = nullptr);  
    ~HeatmapDialog();  

private slots:  
    void onUpdateHeatmap();  

private:  
    QChart*         m_chart;         // 图表  
    QChartView*     m_chartView;     // 图表视图  
    QScatterSeries* m_scatterSeries; // 用于模拟热力图点  
    QValueAxis*     m_axisX;         // X 轴  
    QValueAxis*     m_axisY;         // Y 轴  
    QTimer*         m_timer;         // 定时器  
};  

#endif // HEATMAPDIALOG_H  