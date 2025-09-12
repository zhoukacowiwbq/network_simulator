
/// widgets/TopologyWidget.h
#ifndef TOPOLOGYWIDGET_H
#define TOPOLOGYWIDGET_H

#include <QWidget>
#include <QVector>
#include <QTimer>
#include "TopologyTypes.h"
#include <qstringlist.h>

struct Node;
struct Edge;

class TopologyWidget : public QWidget {
    Q_OBJECT
public:
    explicit TopologyWidget(QWidget *parent = nullptr);
    QStringList getNodesName();

signals:
    void nodechanged(QStringList);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void shakeNodes();

private:
    QVector<Node> nodes;
    QVector<Edge> edges;
    QPointF selectRectTopLeft, selectRectBottomRight, lastMousePos;
    QRectF selectionRect;
    bool selecting = false, dragging = false, selectionActive = false, movingSelection = false;
    QTimer shakeTimer;
    bool connecting = false;
    int sourceNodeIdx = -1;
    QPointF currentMousePos;

    int hitNode(const QPointF &pos) const;
    void clearSelection();
    void removeNode(int idx);
    void saveTopologyToJson();
    void loadTopology(const QString &filename);
};

#endif // TOPOLOGYWIDGET_H




