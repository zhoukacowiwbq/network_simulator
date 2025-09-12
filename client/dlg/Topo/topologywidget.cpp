/// widgets/TopologyWidget.cpp
#include "topologywidget.h"  // defines Node and Edge

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QWidget>
#include <QTimer>
#include <QRandomGenerator>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QString>
#include <QVector>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>

#include "nodeconfigdlg.h"
#include "linkconfigdlg.h"
#include "../../dbmanager.h"

class DBManager;

#define NODE_RADIUS 12

TopologyWidget::TopologyWidget(QWidget *parent)
    : QWidget(parent) {
    loadTopology("topology.json");
    setMouseTracking(true);
    shakeTimer.start(50);
    connect(&shakeTimer, &QTimer::timeout, this, &TopologyWidget::shakeNodes);
}

void TopologyWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    painter.setRenderHint(QPainter::Antialiasing);


    for (const auto &edge : edges) {
        // 1. 端点坐标
        QPointF p1(nodes[edge.src].x, nodes[edge.src].y);
        QPointF p2(nodes[edge.dst].x, nodes[edge.dst].y);

        painter.setPen(QPen(Qt::black, 2));

        // 2. 画线
        painter.drawLine(p1, p2);

        // 3. 计算中点
        QPointF mid = (p1 + p2) * 0.5;

        // 4. 计算单位法向量（将直线向上偏移）
        QPointF dir = p2 - p1;
        double len = std::hypot(dir.x(), dir.y());
        if (len <= 0.0) continue;
        // 法向量：(-dy, dx) 或者 (dy, -dx)，看你想偏移“哪边”
        QPointF norm(-dir.y() / len, dir.x() / len);

        // 5. 文本偏移量（比如偏离 12 像素）
        constexpr double OFFSET = 12.0;
        QPointF textPos = mid + norm * OFFSET;

        // 6. 构造标签内容
        QString label = QString("%1-%2<->%3-%4")
                            .arg(nodes[edge.src].name)
                            .arg(edge.srcIntf)
                            .arg(nodes[edge.dst].name)
                            .arg(edge.dstIntf);

        // 7. 先测量文本尺寸，再居中绘制
        QFontMetrics fm = painter.fontMetrics();
        QRectF  tb  = fm.tightBoundingRect(label);
        tb.moveCenter(textPos);

        // 8. 设置颜色、背景等（可选）
        painter.setPen(Qt::darkBlue);
        painter.drawText(tb, Qt::AlignCenter, label);
    }
    // qDebug() << "edges.size() =" << edges.size();
    // int iter = 0;
    // QVector<Edge> snapshot = edges;
    // for (const auto &edge : snapshot) {
    //     qDebug() << "    loop" << ++iter;
    //     if (iter > snapshot.size()) {
    //         qDebug() << "异常：迭代次数超过 snapshot 大小";
    //         break;
    //     }
    //     painter.drawLine(QPointF(nodes[edge.src].x, nodes[edge.src].y),
    //                      QPointF(nodes[edge.dst].x, nodes[edge.dst].y));
    // }

    // qDebug() << "edges.size() end =" << edges.size();

    if (connecting && sourceNodeIdx != -1) {
        painter.setPen(QPen(Qt::gray, 3, Qt::DashLine));
        painter.drawLine(QPointF(nodes[sourceNodeIdx].x, nodes[sourceNodeIdx].y), currentMousePos);
    }

    for (const auto &node : nodes) {
        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(node.selected ? QColor(255, 0, 0, 50) : QColor(51, 153, 204, 50));
        painter.drawEllipse(QPointF(node.x, node.y), 15, 15);
        painter.setPen(Qt::black);
        int yOffset = 0;
        painter.drawText(QPointF(node.x + 20, node.y + yOffset), tr("Name: %1").arg(node.name));
        yOffset += 15;
        painter.drawText(QPointF(node.x + 20, node.y + yOffset), tr("Loopback: %1").arg(node.loopbackAddress));
        yOffset += 15;
        for (const auto &iface : node.interfaces) {
            QString vlanStr;
            for (int j = 0; j < iface.vlanIDs.size(); ++j) {
                vlanStr += QString::number(iface.vlanIDs.at(j));
                if (j < iface.vlanIDs.size() - 1) vlanStr += ",";
            }
            painter.drawText(QPointF(node.x + 20, node.y + yOffset),
                             tr("Intf: %1, IP: %2, Mode: %3, VLANs: %4")
                                 .arg(iface.name, iface.ipAddress, iface.l2Mode, vlanStr));
            yOffset += 15;
        }
    }

    if (selecting || selectionActive) {
        painter.setPen(QPen(Qt::blue, 1, Qt::DashLine));
        QRectF rect = QRectF(selectRectTopLeft, selectRectBottomRight).normalized();
        painter.drawRect(rect);
    }
}

void TopologyWidget::mousePressEvent(QMouseEvent *event) {
    int idx = hitNode(event->pos());
    if (event->button() == Qt::LeftButton) {
        if (connecting && sourceNodeIdx != -1) {
            if (idx != -1 && idx != sourceNodeIdx) {
                const auto &ifs1 = nodes[sourceNodeIdx].interfaces;
                const auto &ifs2 = nodes[idx].interfaces;

                LinkConfigDlg dlg(this);
                dlg.setNodeNames(nodes[sourceNodeIdx].name, nodes[idx].name);
                dlg.setInterfaceLists(ifs1, ifs2);
                if (dlg.exec() == QDialog::Accepted) {
                    QString if1 = dlg.interface1();
                    QString if2 = dlg.interface2();
                    int     c   = dlg.cost();

                    edges.append({sourceNodeIdx, idx, if1, if2, c, QStringLiteral("default") });
                    connecting = false;
                } 
            }
            update();
        } else if (selectionActive && selectionRect.contains(event->pos())) {
            movingSelection = true;
            dragging = true;
            lastMousePos = event->pos();
            update();
        } else if (idx != -1) {
            clearSelection();
            nodes[idx].selected = true;
            dragging = true;
            lastMousePos = event->pos();
            update();
        } else {
            clearSelection();
            selecting = true;
            selectRectTopLeft = selectRectBottomRight = event->pos();
            update();
        }
    }
}

// ... (rest of TopologyWidget.cpp unchanged, following the same structure for mouseMoveEvent, mouseReleaseEvent, contextMenuEvent, shakeNodes, helper methods)
void TopologyWidget::mouseMoveEvent(QMouseEvent *event) {
    QPointF delta = event->pos() - lastMousePos;

    // 动态连线跟随鼠标，无需按下
    if (connecting && sourceNodeIdx != -1) {
        currentMousePos = event->pos();
        update();
        return; // 连线模式优先，避免冲突
    }

    // 拖动节点或框选区域（需要按下鼠标）
    if (dragging) {
        for (auto &node : nodes) {
            if (node.selected) {
                node.x += delta.x();
                node.y += delta.y();
            }
        }
        if (movingSelection) {
            selectRectTopLeft += delta;
            selectRectBottomRight += delta;
        }
        lastMousePos = event->pos();
        update();
    } else if (selecting) {
        selectRectBottomRight = event->pos();
        update();
    }
}

void TopologyWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (selecting) {
            QRectF rect = QRectF(selectRectTopLeft, selectRectBottomRight).normalized();
            selectionRect = rect;
            for (auto &node : nodes) {
                node.selected = rect.contains(QPointF(node.x, node.y));
            }
            selectionActive = true;
        }
        selecting = dragging = movingSelection = false;
        update();
    }
}

void TopologyWidget::contextMenuEvent(QContextMenuEvent *event) {
    int idx = hitNode(event->pos());

    // 连线模式下，右键直接取消
    if (connecting) {
        connecting = false;
        sourceNodeIdx = -1;
        update();
        return;
    }

    // 正常模式下的右键菜单
    QMenu menu(this);
    if (idx != -1) {
        QAction *configAction = menu.addAction("配置节点");
        QAction *connectAction = menu.addAction("连线");
        QAction *deleteAction = menu.addAction("删除节点");
        QAction *selectedAction = menu.exec(event->globalPos());

        if (selectedAction == configAction) {
            NodeConfigDialog dialog(nodes[idx], this);
            if (dialog.exec() == QDialog::Accepted) {
                update();
            }
        } else if (selectedAction == connectAction) {
            connecting = true;
            sourceNodeIdx = idx;
            currentMousePos = event->pos();
            update();
        } else if (selectedAction == deleteAction) {
            removeNode(idx);
            update();
        }
    } else {
        QAction *addNodeAction = menu.addAction("添加节点");
        QAction *clearAction = menu.addAction("清空拓扑");
        QAction *saveAction = menu.addAction("保存拓扑到JSON");
        QAction *selectedAction = menu.exec(event->globalPos());

        if (selectedAction == addNodeAction) {
            QPointF p = event->pos();
            Node newNode{p.x(), p.y(), false, "新节点", "0.0.0.0", {}};
            nodes.append(newNode);
            update();
        } else if (selectedAction == clearAction) {
            auto ret = QMessageBox::question(this,
            tr("清除所有"),
            tr("确定要清除所有节点和连线吗？此操作无法撤销。"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);
            if (ret == QMessageBox::Yes) {
                nodes.clear();
                edges.clear();
                update();
            }
        } else if (selectedAction == saveAction) {
            saveTopologyToJson();
            update();
        }
    }
}

void TopologyWidget::shakeNodes() {
    if (dragging) {
        for (auto &node : nodes) {
            if (node.selected) {
                node.x += QRandomGenerator::global()->bounded(-1, 2);
                node.y += QRandomGenerator::global()->bounded(-1, 2);
            }
        }
        update();
    }
}

int TopologyWidget::hitNode(const QPointF &pos) const {
    for (int i = 0; i < nodes.size(); ++i) {
        if (QLineF(QPointF(nodes[i].x, nodes[i].y), pos).length() <= NODE_RADIUS)
            return i;
    }
    return -1;
}

void TopologyWidget::clearSelection() {
    for (auto &node : nodes) node.selected = false;
    selectionActive = false;
}

void TopologyWidget::removeNode(int idx) {
    // 删除与该节点相关的边
    for (int i = edges.size() - 1; i >= 0; --i) {
        if (edges[i].src == idx || edges[i].dst == idx) {
            edges.remove(i);
        } else {
            // 更新边的索引
            if (edges[i].src > idx) edges[i].src--;
            if (edges[i].dst > idx) edges[i].dst--;
        }
    }
    // 删除节点
    nodes.remove(idx);
}

void TopologyWidget::saveTopologyToJson() {
    QJsonObject root;
    QJsonObject topology;
    topology["name"] = "Dual Switch Topo"; // 可动态设置

    QJsonArray jnodes;
    for (const auto &node : nodes) {
        QJsonObject jnode;
        jnode["name"] = node.name; // 使用名称作为 id
        jnode["x"] = node.x;
        jnode["y"] = node.y;
        jnode["loopback"] = node.loopbackAddress;
        DBManager::instance().insertNode(node.name,"节点");
        QJsonArray jinterfaces;
        for (const auto &iface : node.interfaces) {
            QJsonObject jiface;
            jiface["name"] = iface.name;
            jiface["ip"] = iface.ipAddress;
            jiface["mode"] = iface.l2Mode; // 改为 mode
            jiface["mask"] = 24;
            DBManager::instance().insertInterface(node.name, iface.name);
            QJsonArray jvlans;
            for (int vlan : iface.vlanIDs) {
                jvlans.append(vlan);
            }
            jiface["vlans"] = jvlans; // 改为 vlans
            jinterfaces.append(jiface);
        }
        jnode["interfaces"] = jinterfaces;
        jnodes.append(jnode);
    }
    topology["nodes"] = jnodes;

    QJsonArray jlinks;
    int link_id = 0;
    for (const auto &edge : edges) {
        QJsonObject jlink;
        jlink["name"] = QString("l%1").arg(link_id++);
        jlink["n1"] = nodes[edge.src].name;
        jlink["n2"] = nodes[edge.dst].name;
        jlink["i1"] = edge.srcIntf;
        jlink["i2"] = edge.dstIntf;
        jlink["cost"] = edge.cost;
        jlink["label"] = QString("%1-%2<->%3-%4")
                             .arg(nodes[edge.src].name, edge.srcIntf,
                                  nodes[edge.dst].name, edge.dstIntf);
        jlinks.append(jlink);
    }
    topology["links"] = jlinks;

    root["topology"] = topology;

    QJsonDocument doc(root);
    QString fileName = QFileDialog::getSaveFileName(this, "保存拓扑", "", "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
        }
    }

    emit nodechanged(getNodesName());
}

void TopologyWidget::loadTopology(const QString &filename) {
    QFile file(filename);

    nodes.clear();
    edges.clear();
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();
    QJsonObject topology = root["topology"].toObject();

    nodes.clear();
    edges.clear();

    QJsonArray jnodes = topology["nodes"].toArray();
    QMap<QString, int> nodeMap; // 名称到索引的映射
    for (int i = 0; i < jnodes.size(); ++i) {
        QJsonObject obj = jnodes[i].toObject();
        Node node;
        node.x = obj["x"].toDouble();
        node.y = obj["y"].toDouble();
        node.selected = false;
        node.name = obj["name"].toString();
        node.loopbackAddress = obj["loopback"].toString();

        QJsonArray jinterfaces = obj["interfaces"].toArray();
        for (const auto &ifaceObj : jinterfaces) {
            QJsonObject ifaceJson = ifaceObj.toObject();
            Interface iface;
            iface.name = ifaceJson["name"].toString();
            iface.ipAddress = ifaceJson["ip"].toString();
            iface.l2Mode = ifaceJson["mode"].toString();
            iface.mask = ifaceJson["mask"].toString();
            QJsonArray jvlans = ifaceJson["vlans"].toArray();
            for (const auto &v : jvlans) {
                iface.vlanIDs.append(v.toInt());
            }
            node.interfaces.append(iface);
        }
        nodes.append(node);
        nodeMap[node.name] = i;
    }

    QJsonArray jlinks = topology["links"].toArray();
    for (const auto &linkObj : jlinks) {
        QJsonObject link = linkObj.toObject();
        Edge edge;
        edge.src = nodeMap[link["n1"].toString()];
        edge.dst = nodeMap[link["n2"].toString()];
        edge.srcIntf = link["i1"].toString();
        edge.dstIntf = link["i2"].toString();
        edge.cost = link["cost"].toInt();
        edges.append(edge);
    }
}


QStringList TopologyWidget::getNodesName() {
    QStringList names;
    names.reserve(nodes.size());          // 预留好空间，避免多次 realloc
    for (const Node &n : nodes) {
        names << n.name;
    }

    return names;
}
