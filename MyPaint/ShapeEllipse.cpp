#include "ShapeEllipse.h"

ShapeEllipse::ShapeEllipse(const QRect& rect)
    : m_rect(rect)
{
}

void ShapeEllipse::paintShape(QPainter* painter)
{
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(Qt::white);
    painter->drawEllipse(m_rect);
}

bool ShapeEllipse::contains(const QPoint& pt) const
{
    // 椭圆的包含判断需要考虑椭圆方程
    QPointF center = m_rect.center();
    double a = m_rect.width() / 2.0;
    double b = m_rect.height() / 2.0;
    double x = (pt.x() - center.x()) / a;
    double y = (pt.y() - center.y()) / b;
    return x * x + y * y <= 1.0;
}

void ShapeEllipse::moveBy(const QPoint& delta)
{
    m_rect.translate(delta);
}

void ShapeEllipse::resize(const QRect& newRect)
{
    m_rect = newRect;
}

QRect ShapeEllipse::boundingRect() const
{
    return m_rect;
}

std::vector<ShapeBase::Handle> ShapeEllipse::getArrowAnchors() const {
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