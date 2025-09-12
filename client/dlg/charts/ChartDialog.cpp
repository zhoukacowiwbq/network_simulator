// TrafficChartDialog.cpp
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDateTime>

#include "ChartDialog.h"


#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis> // Include for Y-axis
#include <algorithm> // For std::max_element

#include "../../dbmanager.h"
// Constructor: Sets up the basic UI and chart structure
TrafficChartDialog::TrafficChartDialog(QWidget *parent) :
    QDialog(parent),
    m_chartView(nullptr),
    m_chart(nullptr),
    m_series(nullptr),
    m_inSet(nullptr),
    m_outSet(nullptr),
    m_axisX(nullptr),
    m_axisY(nullptr) // Initialize Y-axis pointer
{
    setupChart(); // Initialize chart components

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_chartView);
    setLayout(layout);

    setWindowTitle(tr("Node Traffic")); // Optional: Set a title
    setMinimumSize(600, 400);        // Optional: Set a default size
}

// Destructor (optional, Qt parent ownership handles most cleanup)
TrafficChartDialog::~TrafficChartDialog() {
    // Qt's parent-child mechanism usually handles deletion of chart objects
    // when m_chartView or the dialog itself is deleted.
}

// Private helper: Initializes the chart, series, sets, and axes
void TrafficChartDialog::setupChart() {
    m_chart = new QChart();
    m_chart->setTitle(tr("Traffic per Node (Bytes)"));
    m_chart->setAnimationOptions(QChart::SeriesAnimations); // Optional nice animation

    m_series = new QBarSeries();

    // Create the bar sets for incoming and outgoing traffic
    m_inSet = new QBarSet(tr("In"));
    m_outSet = new QBarSet(tr("Out"));

    // Append sets to the series *before* adding series to chart
    m_series->append(m_inSet);
    m_series->append(m_outSet);

    // Add series to the chart
    m_chart->addSeries(m_series);

    // Create and configure the X-axis (categories = node names)
    m_axisX = new QBarCategoryAxis();
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX); // Attach axis to the series

    // Create and configure the Y-axis (values = byte counts)
    m_axisY = new QValueAxis();
    m_axisY->setTitleText(tr("Bytes"));
    m_axisY->setLabelFormat("%lld"); // Use %lld for qint64
    m_axisY->setTickCount(6); // Example: Set number of ticks
    m_axisY->setRange(0, 100); // Initial small range, will be updated
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY); // Attach axis to the series

    // Configure legend
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    // Create the view to display the chart
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing); // Smoother drawing
}

// Slot: Called when the list of nodes changes
void TrafficChartDialog::setNodeNames(const QStringList &names) {
    // 0. Ensure series exists (safety check)
    if (!m_series) {
        qWarning("TrafficChartDialog::setNodeNames called with null series!");
        return;
    }

    // 1. Store the new node names
    m_nodeNames = names;

    // 2. Clear existing counts and resize vectors, initializing with 0
    int nodeCount = m_nodeNames.size();
    m_countsIn.fill(0, nodeCount);
    m_countsOut.fill(0, nodeCount);
    m_countsIn.resize(nodeCount);   // Ensure correct size
    m_countsOut.resize(nodeCount); // Ensure correct size


    // 3. Remove old data from the bar sets using the correct method
    //    QBarSeries::remove() removes AND deletes the set.
    if (m_inSet) {
        m_series->remove(m_inSet); // Removes AND deletes the object
        m_inSet = nullptr;         // Set pointer to null to avoid dangling pointer issues
    }
    if (m_outSet) {
        m_series->remove(m_outSet); // Removes AND deletes the object
        m_outSet = nullptr;         // Set pointer to null
    }

    // Now, m_inSet and m_outSet are nullptr, ready for new sets

    // 4. Create the new sets
    m_inSet = new QBarSet(tr("In"));
    m_outSet = new QBarSet(tr("Out"));

    // Append zero values for each new node
    for (int i = 0; i < nodeCount; ++i) {
        m_inSet->append(0);
        m_outSet->append(0);
    }
    // Append the new sets to the series (series takes ownership again)
    m_series->append(m_inSet);
    m_series->append(m_outSet);


    // 5. Update the X-axis categories
    m_axisX->clear(); // Remove old categories
    m_axisX->append(m_nodeNames); // Add new categories

    // 6. Reset Y-axis range
    updateYAxisRange(); // Adjust based on current (zero) values
}

// Slot: Called when a packet is received
void TrafficChartDialog::recordPacket(const QString &nodeName,
                                      const QString &itf,
                                      const QString &direction,
                                      int length)
{
    Q_UNUSED(itf); // Mark 'itf' as unused if needed

    // 1. Find the index corresponding to the node name
    int index = m_nodeNames.indexOf(nodeName);

    // 2. If node exists in our list, update the counts and the specific bar
    if (index != -1 && index < m_countsIn.size() && index < m_countsOut.size()) {
        bool updated = false;
        if (direction.compare("<--", Qt::CaseInsensitive) == 0) {
            m_countsIn[index] += length;
            // Update the specific bar value in the chart set
            if (index < m_inSet->count()) { // Bounds check
               m_inSet->replace(index, m_countsIn[index]);
               updated = true;
            }
            DBManager::instance().insertTraffic(nodeName, itf, QDateTime::currentDateTime(),length, 0);
        } else if (direction.compare("-->", Qt::CaseInsensitive) == 0) {
            m_countsOut[index] += length;
            // Update the specific bar value in the chart set
            if (index < m_outSet->count()) { // Bounds check
                m_outSet->replace(index, m_countsOut[index]);
                updated = true;
            }
            DBManager::instance().insertTraffic(nodeName, itf, QDateTime::currentDateTime(), 0, length);
        } else {
             // Optional: Handle unknown direction? Log warning?
             // qDebug() << "Unknown direction:" << direction;
        }

        // 3. If a value was updated, recalculate and adjust the Y-axis range
        if (updated) {
            updateYAxisRange();
        }
    } else {
        // Optional: Handle case where nodeName is not found? Log warning?
        // qDebug() << "Node not found in chart:" << nodeName;
    }
}

// Private helper: Adjusts the Y-axis range based on max data value
void TrafficChartDialog::updateYAxisRange() {
    qint64 maxIn = 0;
    if (!m_countsIn.isEmpty()) {
       maxIn = *std::max_element(m_countsIn.constBegin(), m_countsIn.constEnd());
    }

    qint64 maxOut = 0;
     if (!m_countsOut.isEmpty()) {
       maxOut = *std::max_element(m_countsOut.constBegin(), m_countsOut.constEnd());
    }

    qint64 overallMax = std::max(maxIn, maxOut);

    // Set a reasonable upper limit (e.g., 10% padding, or minimum 100)
    qint64 upperLimit = std::max(static_cast<qint64>(100), static_cast<qint64>(overallMax * 1.1));

    // Apply the new range to the Y-axis
    if (m_axisY) { // Check if axis exists
        m_axisY->setRange(0, upperLimit);
    }
}


