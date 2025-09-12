#include "dbmanager.h"
#include <QDebug>

DBManager& DBManager::instance() {
    static DBManager inst;
    return inst;
}

DBManager::DBManager() { }

DBManager::~DBManager() {
    if (db.isOpen())
        db.close();
}

bool DBManager::initDatabase(const QString& host,
                             int port,
                             const QString& dbName,
                             const QString& user,
                             const QString& passwd)
{
    if (QSqlDatabase::contains("qt_conn"))
        db = QSqlDatabase::database("qt_conn");
    else {
        db = QSqlDatabase::addDatabase("QMYSQL", "qt_conn");
        db.setHostName(host);
        db.setPort(port);
        db.setDatabaseName(dbName);
        db.setUserName(user);
        db.setPassword(passwd);
    }
    if (!db.open()) {
        qCritical() << "Cannot open MySQL:" << db.lastError().text();
        return false;
    }
    return createTables();
}

bool DBManager::createTables() {
    QSqlQuery q(db);
    // nodes
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS nodes (
          node_id     INT NOT NULL AUTO_INCREMENT,
          node_name   VARCHAR(64) NOT NULL UNIQUE,
          description TEXT,
          PRIMARY KEY (node_id)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
    )")) return false;
    // interfaces
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS interfaces (
          interface_id   INT NOT NULL AUTO_INCREMENT,
          node_id        INT NOT NULL,
          interface_name VARCHAR(64) NOT NULL,
          PRIMARY KEY (interface_id),
          KEY idx_interfaces_node (node_id),
          CONSTRAINT fk_interfaces_node
            FOREIGN KEY (node_id) REFERENCES nodes(node_id)
            ON DELETE CASCADE
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
    )")) return false;
    // packets
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS packets (
          packet_id    BIGINT NOT NULL AUTO_INCREMENT,
          interface_id INT     NOT NULL,
          direction    ENUM('←','→') NOT NULL,
          timestamp    DATETIME NOT NULL,
          src_mac      VARCHAR(17) NOT NULL,
          dst_mac      VARCHAR(17) NOT NULL,
          protocol     VARCHAR(16) NOT NULL,
          length       INT     NOT NULL,
          info         TEXT,
          PRIMARY KEY (packet_id),
          KEY idx_packets_if (interface_id),
          CONSTRAINT fk_packets_if
            FOREIGN KEY (interface_id) REFERENCES interfaces(interface_id)
            ON DELETE CASCADE
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
    )")) return false;
    // traffic
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS traffic (
          traffic_id   BIGINT NOT NULL AUTO_INCREMENT,
          interface_id INT     NOT NULL,
          timestamp    DATETIME NOT NULL,
          bytes_in     BIGINT  NOT NULL,
          bytes_out    BIGINT  NOT NULL,
          PRIMARY KEY (traffic_id),
          UNIQUE KEY ux_traffic_iface (interface_id),
          CONSTRAINT fk_traffic_if
            FOREIGN KEY (interface_id) REFERENCES interfaces(interface_id)
            ON DELETE CASCADE
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
    )")) return false;

    return true;
}

bool DBManager::insertNode(const QString& nodeName, const QString& desc) {
    QSqlQuery q(db);
    q.prepare("INSERT IGNORE INTO nodes(node_name, description) VALUES(:n,:d)");
    q.bindValue(":n", nodeName);
    q.bindValue(":d", desc);
    return q.exec();
}

int DBManager::nodeId(const QString& nodeName) {
    QSqlQuery q(db);
    q.prepare("SELECT node_id FROM nodes WHERE node_name=:n");
    q.bindValue(":n", nodeName);
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return -1;
}

bool DBManager::insertInterface(const QString& nodeName, const QString& itfName) {
    int nid = nodeId(nodeName);
    if (nid < 0) return false;
    QSqlQuery q(db);
    q.prepare("INSERT IGNORE INTO interfaces(node_id, interface_name) "
              "VALUES(:nid, :ifn)");
    q.bindValue(":nid", nid);
    q.bindValue(":ifn", itfName);
    return q.exec();
}

int DBManager::interfaceId(const QString& nodeName, const QString& itfName) {
    int nid = nodeId(nodeName);
    if (nid < 0) return -1;
    QSqlQuery q(db);
    q.prepare("SELECT interface_id FROM interfaces "
              "WHERE node_id=:nid AND interface_name=:ifn");
    q.bindValue(":nid", nid);
    q.bindValue(":ifn", itfName);
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return -1;
}

bool DBManager::insertPacket(const QString& nodeName,
                             const QString& itfName,
                             const QString& direction,
                             const QString& ts,
                             const QString& srcMac,
                             const QString& dstMac,
                             const QString& protocol,
                             int length,
                             const QString& info)
{
    int iid = interfaceId(nodeName, itfName);
    if (iid < 0) return false;

    QSqlQuery q(db);
    q.prepare(R"(
        INSERT INTO packets
        (interface_id, direction, timestamp, src_mac, dst_mac, protocol, length, info)
        VALUES(:ii, :dir, :ts, :sm, :dm, :prot, :len, :inf)
    )");
    q.bindValue(":ii",  iid);
    q.bindValue(":dir", direction);
    q.bindValue(":ts",  ts);
    q.bindValue(":sm",  srcMac);
    q.bindValue(":dm",  dstMac);
    q.bindValue(":prot", protocol);
    q.bindValue(":len", length);
    q.bindValue(":inf", info);
    return q.exec();
}

bool DBManager::insertTraffic(const QString& nodeName,
                              const QString& itfName,
                              const QDateTime& ts,
                              qint64 bytesIn,
                              qint64 bytesOut)
{
    int iid = interfaceId(nodeName, itfName);
    if (iid < 0) return false;

    QSqlQuery q(db);
    q.prepare(R"(
      INSERT INTO traffic(interface_id, timestamp, bytes_in, bytes_out)
        VALUES(:ii, :ts, :in, :out)
      ON DUPLICATE KEY UPDATE
        bytes_in    = bytes_in  + VALUES(bytes_in),
        bytes_out   = bytes_out + VALUES(bytes_out),
        timestamp = VALUES(timestamp)
    )");
    q.bindValue(":ii",  iid);
    q.bindValue(":ts",  ts.toString("yyyy-MM-dd HH:mm:ss"));
    q.bindValue(":in",  bytesIn);
    q.bindValue(":out", bytesOut);
    return q.exec();
}
