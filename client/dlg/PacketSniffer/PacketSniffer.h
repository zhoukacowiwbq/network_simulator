#ifndef PACKETSNIFFER_H
#define PACKETSNIFFER_H

#include <QWidget>
#include <QTcpServer>
#include <QStandardItemModel>
#include <QTableView>
#include <QList>
#include <QByteArray>

class QTcpSocket;

class PacketSniffer : public QWidget {
    Q_OBJECT
public:
    explicit PacketSniffer(QWidget* parent = nullptr);
    virtual ~PacketSniffer();

signals:
    void packetReceived(const QString& node, const QString& intf, const QString& direction, int length);

protected:
    // 虚函数，允许子类重写数据包处理逻辑
    virtual void processPacket(const QString& packet);

public slots:
    void clearTable();

private slots:
    void onNewConnection();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpServer* m_server;
    QStandardItemModel* m_model;
    QTableView* m_tableView;
    QList<QTcpSocket*> m_sockets;
    QByteArray m_buffer;
};

#endif // PACKETSNIFFER_H