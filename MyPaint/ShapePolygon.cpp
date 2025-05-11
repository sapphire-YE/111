#include "ShapePolygon.h"
#include <QPainter>

ShapePolygon::ShapePolygon(const QPolygon& polygon)
    : m_polygon(polygon)
{
}

void ShapePolygon::paintShape(QPainter* painter)
{
    if (!painter) return;
    painter->drawPolygon(m_polygon);
}

bool ShapePolygon::contains(const QPoint& pt) const
{
    return m_polygon.containsPoint(pt, Qt::OddEvenFill);
}

void ShapePolygon::moveBy(const QPoint& delta)
{
    m_polygon.translate(delta);
}

void ShapePolygon::resize(const QRect& newRect)
{

    // 这里使用简单的实现，将多边形缩放到新矩形
    QRect oldRect = boundingRect();
    if (oldRect.isEmpty()) return;

    QPolygon newPolygon;
    for (const QPoint& pt : m_polygon) {
        int newX = newRect.left() + (pt.x() - oldRect.left()) * newRect.width() / oldRect.width();
        int newY = newRect.top() + (pt.y() - oldRect.top()) * newRect.height() / oldRect.height();
        newPolygon << QPoint(newX, newY);
    }
    m_polygon = newPolygon;
}

QRect ShapePolygon::boundingRect() const
{
    return m_polygon.boundingRect();
} 