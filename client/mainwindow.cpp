#include "mainwindow.h"
#include "dlg/console/circularconsole.h"
#include "dlg/charts/ChartDialog.h"  
#include "dlg/table/DBDataDialog.h"  
#include "dlg/Bar/BarChartDialog.h"
#include "dlg/HeatMap/HeatmapDialog.h"
#include "dlg/PacketSniffer/PacketSniffer.h"
#include "dlg/Topo/topologywidget.h"
#include <QVBoxLayout>

#include <QWidgetAction>
#include <QLabel>
#include <QCalendarWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QDockWidget>

// #include "ads/DockAreaWidget.h"
// #include "ads/DockAreaTitleBar.h"
// #include "ads/DockAreaTabBar.h"
// #include "ads/FloatingDockContainer.h"
// #include "ads/DockComponentsFactory.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto *console = new CircularConsole(this);
    setCentralWidget(console);

    constexpr Qt::DockWidgetArea allAreas = Qt::DockWidgetArea::AllDockWidgetAreas; 

    auto *snifferDock = new QDockWidget(tr("数据包捕获窗口"), this);
    snifferDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    snifferDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    auto *sniffer = new PacketSniffer(this);
    snifferDock->setWidget(sniffer);
    snifferDock->setMinimumSize(300, 200);
    addDockWidget(Qt::BottomDockWidgetArea, snifferDock);

    auto *chartDock = new QDockWidget(tr("图表"), this);
    chartDock->setAllowedAreas(allAreas);
    chartDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    auto *chart = new TrafficChartDialog(this);
    chartDock->setWidget(chart);
    chartDock->setMinimumSize(400, 300);
    addDockWidget(Qt::LeftDockWidgetArea, chartDock);

    connect(sniffer, &PacketSniffer::packetReceived,
            chart, &TrafficChartDialog::recordPacket);

    auto *topoDock = new QDockWidget(tr("拓扑"), this);
    topoDock->setAllowedAreas(allAreas);
    topoDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    auto *topo = new TopologyWidget(this);
    topoDock->setWidget(topo);
    topoDock->setMinimumSize(400, 300);
    addDockWidget(Qt::TopDockWidgetArea, topoDock);

    connect(console, &CircularConsole::clear,
            sniffer, &PacketSniffer::clearTable);
    connect(topo, &TopologyWidget::nodechanged,
            chart, &TrafficChartDialog::setNodeNames);

    
}

MainWindow::~MainWindow() {}


