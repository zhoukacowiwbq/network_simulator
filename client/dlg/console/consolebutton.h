#ifndef CONSOLEBUTTON_H
#define CONSOLEBUTTON_H

#include <QGraphicsObject>
#include <QPixmap>

class ConsoleButton : public QGraphicsObject
{
    Q_OBJECT
public:
    ConsoleButton(qreal startAngle, qreal spanAngle, qreal innerRadius, qreal outerRadius);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setSelected(bool selected);
    void setIcon(const QPixmap &icon);
    void setAngles(qreal startAngle, qreal spanAngle);
    void setRadii(qreal innerRadius, qreal outerRadius);
    void setButtonText(QString text) {m_buttonText = text;};
    QString buttonText() {return m_buttonText;};
    QPainterPath shape() const override;

signals:
    void clicked(ConsoleButton* self);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    qreal m_startAngle;
    qreal m_spanAngle;
    qreal m_innerRadius;
    qreal m_outerRadius;
    bool m_selected;
    bool m_hovered;
    QPixmap m_icon;
    QPointF mapFromScene(const QPointF &point) const;
    QString m_buttonText;
};

#endif // CONSOLEBUTTON_H
