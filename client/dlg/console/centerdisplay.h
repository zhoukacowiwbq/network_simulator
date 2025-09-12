#ifndef CENTERDISPLAY_H
#define CENTERDISPLAY_H

#include <QGraphicsItem>
#include <QPixmap>


class CenterDisplay : public QGraphicsItem
{
public:
    explicit CenterDisplay(qreal radius);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setIcon(const QPixmap &icon);
    void setRadius(qreal radius);

private:
    qreal m_radius;
    QPixmap m_icon;
};
#endif // CENTERDISPLAY_H
