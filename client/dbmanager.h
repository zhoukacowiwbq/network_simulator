#pragma once

#include <QtSql>
#include <QDateTime>

class DBManager {
public:
    static DBManager& instance();

    /// 初始化并创建表结构
    bool initDatabase(const QString& host,
                      int port,
                      const QString& dbName,
                      const QString& user,
                      const QString& passwd);

    /// 插入或忽略已有节点
    bool insertNode(const QString& nodeName,
                    const QString& desc = QString());

    /// 根据节点名查 node_id
    int nodeId(const QString& nodeName);

    /// 插入接口（自动创建所属节点）
    bool insertInterface(const QString& nodeName,
                         const QString& itfName);

    /// 根据节点名+接口名查 interface_id
    int interfaceId(const QString& nodeName,
                    const QString& itfName);

    /// 插入一条数据包记录
    bool insertPacket(const QString& nodeName,
                      const QString& itfName,
                      const QString& direction,
                      const QString& ts,
                      const QString& srcMac,
                      const QString& dstMac,
                      const QString& protocol,
                      int length,
                      const QString& info);

    /// 插入一条流量统计记录
    bool insertTraffic(const QString& nodeName,
                       const QString& itfName,
                       const QDateTime& ts,
                       qint64 bytesIn,
                       qint64 bytesOut);

private:
    QSqlDatabase db;
    DBManager();
    ~DBManager();
    Q_DISABLE_COPY(DBManager)

    /// 在打开连接后创建所有表
    bool createTables();
};

