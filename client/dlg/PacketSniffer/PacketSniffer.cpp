#include "PacketSniffer.h"
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include <QHeaderView>
#include "../../dbmanager.h"

PacketSniffer::PacketSniffer(QWidget* parent)
    : QWidget(parent)
    , m_server(new QTcpServer(this))
    , m_model(new QStandardItemModel(this))
    , m_tableView(new QTableView(this))
    , m_buffer()
{
    // 初始化表格
    m_model->setHorizontalHeaderLabels({"Time", "Source MAC", "Destination MAC", "Protocol", "Length", "Source IP", "Destination IP", "Info"});
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 设置自动调整宽度
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); // 允许用户调整列宽

    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_tableView);
    setLayout(layout);

    // 初始化TCP服务器
    if (!m_server->listen(QHostAddress::Any, 55555)) {
        qDebug() << "Server failed to start:" << m_server->errorString();
    }

    // 连接信号
    connect(m_server, &QTcpServer::newConnection, this, &PacketSniffer::onNewConnection);
}

PacketSniffer::~PacketSniffer() {}

void PacketSniffer::onNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(socket, &QTcpSocket::readyRead, this, &PacketSniffer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &PacketSniffer::onDisconnected);
    m_sockets.append(socket);
    qDebug() << "New connection from" << socket->peerAddress().toString();
}

void PacketSniffer::onDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    m_sockets.removeOne(socket);
    socket->deleteLater();
    qDebug() << "Disconnected from" << socket->peerAddress().toString();
}

void PacketSniffer::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    qDebug()<< "before append: "<< m_buffer;
    m_buffer.append(socket->readAll());

    while (true) {
        int packet_end = m_buffer.indexOf("\n\n");
        if (packet_end == -1) {
             // 如果没有找到分隔符，检查 m_buffer 是否以单个 \n 结尾
            if (m_buffer.endsWith("\n")) {
                // 如果以单个 \n 结尾，表示当前包是最后一个包，处理它
                QString packet = QString::fromUtf8(m_buffer);
                processPacket(packet);
                m_buffer.clear();  // 清空缓冲区，因为已经处理了这个包
                break;
            }
            break;
        }

        QString packet = QString::fromUtf8(m_buffer.mid(0, packet_end));
        m_buffer.remove(0, packet_end + 2);

        processPacket(packet);
    }
}

void PacketSniffer::processPacket(const QString& packet)
{
    QStringList lines = packet.split('\n', Qt::SkipEmptyParts);
    if (lines.isEmpty()) return;

    // 第一行：节点、接口、方向
    QString node, intf, direction;
    QRegularExpression re_first(R"((\w+)\((\w+)\)\s+([<>-]+))");
    QRegularExpressionMatch match = re_first.match(lines[0]);
    if (match.hasMatch()) {
        node = match.captured(1);
        intf = match.captured(2);
        direction = match.captured(3);
    } else {
        qDebug() << "Invalid first line:" << lines[0];
        return;
    }

    // 第二行：以太网头
    QString src_mac, dst_mac, protocol, vlan, length;
    if (lines.size() > 1) {
        QRegularExpression re_eth(R"(Eth hdr\s*:\s*([\w:]+)\s*->\s*([\w:]+)\s*(\S+)\s*Vlan:\s*(\d+)\s*PL:\s*(\d+)B)");
        match = re_eth.match(lines[1]);
        if (match.hasMatch()) {
            src_mac = match.captured(1);
            dst_mac = match.captured(2);
            protocol = match.captured(3);
            vlan = match.captured(4);
            length = match.captured(5);
        }
    }

    // 第三行（可选）：ARP或IP头
    QString info = QString("%1(%2) %3").arg(node, intf, direction);
    QString src_ip, dst_ip;
    if (lines.size() > 2) {
        if (lines[2].contains("ARP Hdr")) {
            QRegularExpression re_arp(R"(Arp Type:\s*(\S+)\s*([\w:]+)\s*->\s*([\w:]+)\s*([\d.]+)\s*->\s*([\d.]+))");
            match = re_arp.match(lines[2]);
            if (match.hasMatch()) {
                QString arp_type = match.captured(1);
                src_ip = match.captured(4);
                dst_ip = match.captured(5);
                info += QString(" %1 %2 -> %3").arg(arp_type, src_ip, dst_ip);
            }
            protocol = "ARP";
        } else if (lines[2].contains("IP Hdr")) {
            QRegularExpression re_ip(R"(TL:\s*(\S+)\s*PRO:\s*(\S+)\s*([\d.]+)\s*->\s*([\d.]+)\s*ttl:\s*(\d+))");
            match = re_ip.match(lines[2]);
            if (match.hasMatch()) {
                QString pro = match.captured(2);
                src_ip = match.captured(3);
                dst_ip = match.captured(4);
                QString ttl = match.captured(5);
                info += QString(" %1 %2 -> %3 ttl: %4").arg(pro, src_ip, dst_ip, ttl);
            }
            protocol = "IP";
        }
    }

    // 生成时间戳
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss");

    // 添加到表格的顶部
    QList<QStandardItem*> row;
    row << new QStandardItem(timestamp)
        << new QStandardItem(src_mac)
        << new QStandardItem(dst_mac)
        << new QStandardItem(protocol)
        << new QStandardItem(length)
        << new QStandardItem(src_ip)  // 新增 Source IP 列
        << new QStandardItem(dst_ip)  // 新增 Destination IP 列
        << new QStandardItem(info);
    m_model->insertRow(0, row);  // 按倒序插入
    bool ok;
    int lengthValue = length.toInt(&ok);

    DBManager::instance().insertPacket(node, intf, direction, timestamp, src_mac, dst_mac, protocol, lengthValue, info);
    // 发射信号给 TrafficChartDialog
    if (ok) {
        emit packetReceived(node, intf, direction, lengthValue);
    } else {
        qDebug() << "Invalid length:" << length;
    }
}

// 新增的槽函数，清空表格
void PacketSniffer::clearTable()
{
    m_model->removeRows(0, m_model->rowCount());
}
