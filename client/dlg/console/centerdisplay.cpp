#include "centerdisplay.h"
#include <QPainter>

CenterDisplay::CenterDisplay(qreal radius) : m_radius(radius) {}

QRectF CenterDisplay::boundingRect() const
{
    return QRectF(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
}

void CenterDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QColor(40, 40, 40));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(boundingRect());

    if (!m_icon.isNull()) {
        qreal iconSize = m_radius * 1.4;
        QRectF iconRect(-iconSize/2, -iconSize/2, iconSize, iconSize);
        painter->drawPixmap(iconRect, m_icon, m_icon.rect());
    }
}

void CenterDisplay::setIcon(const QPixmap &icon)
{
    m_icon = icon;
    update();
}

void CenterDisplay::setRadius(qreal radius)
{
    m_radius = radius;
    update();
}
