#include "ShapeRect.h"

ShapeRect::ShapeRect(const QRect& rect)
    : m_rect(rect)
{
}

void ShapeRect::paintShape(QPainter* painter)
{
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(Qt::white);
    painter->drawRect(m_rect);
}

bool ShapeRect::contains(const QPoint& pt) const
{
    return m_rect.contains(pt);
}

void ShapeRect::moveBy(const QPoint& delta)
{
    m_rect.translate(delta);
}

void ShapeRect::resize(const QRect& newRect)
{
    m_rect = newRect;
}

QRect ShapeRect::boundingRect() const
{
    return m_rect;
}

std::vector<ShapeBase::Handle> ShapeRect::getArrowAnchors() const {
    std::vector<Handle> anchors;
    QRect rect = m_rect;
    int w = rect.width(), h = rect.height();
    int x = rect.left(), y = rect.top();
    int size = 8;
    // 上
    anchors.push_back({QRect(x + w/2 - size/2, y - size/2, size, size), Handle::ArrowAnchor, 0});
    // 下
    anchors.push_back({QRect(x + w/2 - size/2, y + h - size/2, size, size), Handle::ArrowAnchor, 1});
    // 左
    anchors.push_back({QRect(x - size/2, y + h/2 - size/2, size, size), Handle::ArrowAnchor, 2});
    // 右
    anchors.push_back({QRect(x + w - size/2, y + h/2 - size/2, size, size), Handle::ArrowAnchor, 3});
    return anchors;
} 