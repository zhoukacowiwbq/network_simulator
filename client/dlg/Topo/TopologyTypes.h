#ifndef TOPOLOGY_TYPES_H
#define TOPOLOGY_TYPES_H

#include <QString>
#include <QVector>
#include <QPointF>

// 一个接口（Interface）上的 L2 模式和 VLAN 列表
struct Interface {
    QString name;
    QString ipAddress;
    QString l2Mode;
    QString mask;
    QVector<int> vlanIDs;
};

// 拓扑中的一个节点
struct Node {
    double x;                        // 屏幕坐标 X
    double y;                        // 屏幕坐标 Y
    bool   selected{false};         // 选中状态
    QString name;                   // 节点显示名称
    QString loopbackAddress;        // 回环地址
    QVector<Interface> interfaces;   // 接口列表
};

// 拓扑中的一条链路（Edge）
struct Edge {
    int    src;       // 源节点在 Node 列表中的索引
    int    dst;       // 目的节点在 Node 列表中的索引
    QString srcIntf;  // 源接口名（如 "eth0"）
    QString dstIntf;  // 目的接口名
    int    cost;      // 代价
    QString name;
};

#endif // TOPOLOGY_TYPES_H
