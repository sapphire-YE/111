#define _USE_MATH_DEFINES
#include "ShapeArrow.h"
#include <QPainter>
#include <cmath>
//#include <QDebug>

ShapeArrow::ShapeArrow(const QLine& line)
    : m_line(line)
{
}

void ShapeArrow::paintShape(QPainter* painter)
{
    if (!painter) return;

    // 设置黑色画笔
    painter->setPen(QPen(Qt::black, 1));

    // 绘制箭头线
    painter->drawLine(m_line);

    // 计算箭头角度
    double angle = atan2(m_line.y2() - m_line.y1(), m_line.x2() - m_line.x1());
    
    // 箭头大小
    const int arrowSize = 10;
    
    // 计算箭头点
    QPoint arrowP1 = m_line.p2() - QPoint(
        arrowSize * cos(angle + M_PI/6),
        arrowSize * sin(angle + M_PI/6)
    );
    QPoint arrowP2 = m_line.p2() - QPoint(
        arrowSize * cos(angle - M_PI/6),
        arrowSize * sin(angle - M_PI/6)
    );

    // 绘制箭头
    painter->drawLine(m_line.p2(), arrowP1);
    painter->drawLine(m_line.p2(), arrowP2);
}

bool ShapeArrow::contains(const QPoint& pt) const
{
    // 简单的点击检测，可以改进为更精确的检测
    return boundingRect().contains(pt);
}

void ShapeArrow::moveBy(const QPoint& delta)
{
    m_line.translate(delta);
}

void ShapeArrow::resize(const QRect& newRect)
{
    // 空实现，保留接口兼容基类
}

QRect ShapeArrow::boundingRect() const
{
    return QRect(m_line.p1(), m_line.p2()).normalized().adjusted(-5, -5, 5, 5);
}

// 新增：获取起点和终点的锚点
std::vector<ShapeBase::Handle> ShapeArrow::getHandles() const
{
    std::vector<Handle> handles;
    int size = 8;  // 锚点大小

    // 起点锚点
    handles.push_back({
        QRect(m_line.p1().x() - size/2, m_line.p1().y() - size/2, size, size),
        Handle::Scale,
        0
    });

    // 终点锚点
    handles.push_back({
        QRect(m_line.p2().x() - size/2, m_line.p2().y() - size/2, size, size),
        Handle::Scale,
        1
    });

    return handles;
}

bool ShapeArrow::needPlusHandles() const { return false; } 