#ifndef TRAFFICCHARTDIALOG_H
#define TRAFFICCHARTDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QVector>

// Forward declarations for QtCharts classes
QT_BEGIN_NAMESPACE
class QChartView;
class QChart;
class QBarSeries;
class QBarSet;
class QBarCategoryAxis;
class QValueAxis; // Added for Y-axis access
QT_END_NAMESPACE

class TrafficChartDialog : public QDialog {
    Q_OBJECT

public:
    explicit TrafficChartDialog(QWidget *parent = nullptr);
    ~TrafficChartDialog(); // Add destructor for cleanup if needed

public slots:
    // Record packet data, update counts and chart display
    void recordPacket(const QString &nodeName,
                      const QString &itf, // Kept as requested, but unused in logic
                      const QString &direction,
                      int length);

    // Reset node list, clear chart data, and rebuild categories
    void setNodeNames(const QStringList &names);

private:
    void setupChart(); // Helper to initialize chart components
    void updateYAxisRange(); // Helper to adjust Y-axis based on current max value

    QChartView       *m_chartView;
    QChart           *m_chart;
    QBarSeries       *m_series;
    QBarSet          *m_inSet;    // Bar set for incoming traffic
    QBarSet          *m_outSet;   // Bar set for outgoing traffic
    QBarCategoryAxis *m_axisX;   // X-axis (node names)
    QValueAxis       *m_axisY;   // Y-axis (byte counts) - Added member

    QStringList       m_nodeNames; // List of current node names
    QVector<qint64>   m_countsIn;  // Use qint64 for potentially large byte counts
    QVector<qint64>   m_countsOut; // Use qint64
};

#endif // TRAFFICCHARTDIALOG_H
