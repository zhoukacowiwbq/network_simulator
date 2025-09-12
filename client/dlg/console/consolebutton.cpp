#include "consolebutton.h"
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

ConsoleButton::ConsoleButton(qreal startAngle, qreal spanAngle, qreal innerRadius, qreal outerRadius)
    : m_startAngle(startAngle), m_spanAngle(spanAngle), m_innerRadius(innerRadius), m_outerRadius(outerRadius),
    m_selected(false), m_hovered(false)
{
    setAcceptHoverEvents(true);
    setTransformOriginPoint(0, 0);  // 设置变换原点为(0,0)
}

QRectF ConsoleButton::boundingRect() const
{
    qreal radius = qMax(m_outerRadius, m_innerRadius);
    return QRectF(-radius, -radius, 2*radius, 2*radius);
}

void ConsoleButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    qreal startAngleDegrees = m_startAngle * 180 / M_PI;
    qreal spanAngleDegrees = m_spanAngle * 180 / M_PI;

    path.moveTo(m_innerRadius * qCos(m_startAngle), -m_innerRadius * qSin(m_startAngle));
    path.arcTo(QRectF(-m_innerRadius, -m_innerRadius, 2*m_innerRadius, 2*m_innerRadius),
               startAngleDegrees, spanAngleDegrees);
    path.lineTo(m_outerRadius * qCos(m_startAngle + m_spanAngle), -m_outerRadius * qSin(m_startAngle + m_spanAngle));
    path.arcTo(QRectF(-m_outerRadius, -m_outerRadius, 2*m_outerRadius, 2*m_outerRadius),
               startAngleDegrees + spanAngleDegrees, -spanAngleDegrees);
    path.closeSubpath();

    // 设置按钮颜色
    QColor buttonColor;
    if (m_selected) {
    } else if (m_hovered) {
        buttonColor = QColor(50, 129, 204, 128);
    } else {
        buttonColor = QColor(60, 60, 60);
    }

    // 绘制按钮主体
    painter->setBrush(buttonColor);
    painter->setPen(Qt::NoPen);
    painter->drawPath(path);

    // 如果按钮被选中或悬停，添加发光效果
    if (m_hovered) {
        QPen glowPen(Qt::white, 2);
        glowPen.setStyle(Qt::SolidLine);
        painter->setPen(glowPen);
        painter->drawPath(path);
    }

    // 绘制图标（如果有）
    if (!m_icon.isNull()) {
        qreal iconSize = (m_outerRadius - m_innerRadius) * 0.6;
        qreal centerRadius = (m_innerRadius + m_outerRadius) / 2;
        qreal centerAngle = m_startAngle + m_spanAngle / 2;
        qreal centerX = centerRadius * qCos(centerAngle);
        qreal centerY = -centerRadius * qSin(centerAngle);
        QRectF iconRect(centerX - iconSize/2, centerY - iconSize/2, iconSize, iconSize);
        painter->drawPixmap(iconRect, m_icon, m_icon.rect());
    }

    if (!m_buttonText.isEmpty()) {  
        // 根据需要设置字体、大小、颜色  
        QFont font = painter->font();  
        font.setPointSize(14);          // 字体大小可根据需求调整  
        font.setFamily("Segoe UI, Microsoft YaHei");  
        font.setBold(5);       
        painter->setFont(font);  
        painter->setPen(QColor("#F2E6CC"));     // 白色字体  

        // 计算文本的绘制中心，和图标类似  
        qreal centerRadius = (m_innerRadius + m_outerRadius) / 2;  
        qreal centerAngle = m_startAngle + m_spanAngle / 2;  
        qreal centerTextX = centerRadius * qCos(centerAngle);  
        qreal centerTextY = -centerRadius * qSin(centerAngle);  

        // 如果需要把文字画在图标下面，可以在 y 方向上做适当偏移  
        qreal textOffset =  (m_outerRadius - m_innerRadius) * 0.3;  
        centerTextY += textOffset;  

        // 设定一个合适的小矩形来让 drawText 居中  
        QRectF textRect(centerTextX - 40, centerTextY - 15, 80, 30);  
        painter->drawText(textRect, Qt::AlignCenter, m_buttonText);  
    }  

    // painter->setPen(QPen(Qt::red, 1));
    // painter->setBrush(Qt::NoBrush);
    // painter->drawPath(shape());
}

void ConsoleButton::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

void ConsoleButton::setIcon(const QPixmap &icon)
{
    m_icon = icon;
    update();
}

void ConsoleButton::setAngles(qreal startAngle, qreal spanAngle)
{
    m_startAngle = startAngle;
    m_spanAngle = spanAngle;
    update();
}

void ConsoleButton::setRadii(qreal innerRadius, qreal outerRadius)
{
    m_innerRadius = innerRadius;
    m_outerRadius = outerRadius;
    update();
}

void ConsoleButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << "enter out";
    //if (shape().contains(mapFromScene(event->scenePos()))) {
        qDebug() << "enter";
        m_hovered = true;
        update();
   // }
    QGraphicsItem::hoverEnterEvent(event);
}

void ConsoleButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
   // if (!shape().contains(mapFromScene(event->scenePos()))) {
        qDebug() << "leave";
        m_hovered = false;
        update();
   // }
    QGraphicsItem::hoverLeaveEvent(event);
}

void ConsoleButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked(this);
    if (shape().contains(mapFromScene(event->scenePos()))) {
        setSelected(!m_selected); // 切换选中状态
        update();
    }
    QGraphicsItem::mousePressEvent(event);
}

QPainterPath ConsoleButton::shape() const
{
    QPainterPath path;
    path.moveTo(m_innerRadius * qCos(m_startAngle), -m_innerRadius * qSin(m_startAngle));
    path.arcTo(QRectF(-m_innerRadius, -m_innerRadius, 2*m_innerRadius, 2*m_innerRadius),
               m_startAngle * 180 / M_PI, m_spanAngle * 180 / M_PI);
    path.lineTo(m_outerRadius * qCos(m_startAngle + m_spanAngle), -m_outerRadius * qSin(m_startAngle + m_spanAngle));
    path.arcTo(QRectF(-m_outerRadius, -m_outerRadius, 2*m_outerRadius, 2*m_outerRadius),
               (m_startAngle + m_spanAngle) * 180 / M_PI, -m_spanAngle * 180 / M_PI);
    path.closeSubpath();
    return path;
}

QPointF ConsoleButton::mapFromScene(const QPointF &point) const
{
    // 将场景坐标转换为按钮的本地坐标
    return QPointF(point.x() - pos().x(), point.y() - pos().y());
}
