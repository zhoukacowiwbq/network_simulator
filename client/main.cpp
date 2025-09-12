#include <QApplication>
#include "mainwindow.h"
#include <QString>
#include "dlg/MyCustomStyle.h"
#include "dbmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DBManager &db = DBManager::instance();
   /* if (!db.initDatabase("192.168.1.13", 3306, "network_monitor","qtuser", "qtpass")) {
        qFatal("无法打开数据库！");
        return -1;
    }*/

    MainWindow w;
    w.show();
    // CircularConsole console;
    w.resize(800, 800);
    // console.show();
    return a.exec();
}
