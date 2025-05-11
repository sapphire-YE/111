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
    // 考虑旋转后的点击检测
    if (m_rotation == 0.0) {
        // 计算点到线段的距离
        QPointF p1(m_line.p1());
        QPointF p2(m_line.p2());
        QPointF p(pt);
        
        // 计算线段向量
        QPointF line = p2 - p1;
        double lineLength = sqrt(line.x() * line.x() + line.y() * line.y());
        if (lineLength < 1e-6) return false;  // 线段太短
        
        // 计算点到线段的投影
        QPointF lineNormalized = line / lineLength;
        QPointF toPoint = p - p1;
        double projection = toPoint.x() * lineNormalized.x() + toPoint.y() * lineNormalized.y();
        
        // 检查投影是否在线段范围内
        if (projection < 0 || projection > lineLength) return false;
        
        // 计算点到线段的垂直距离
        QPointF projectionPoint = p1 + lineNormalized * projection;
        QPointF diff = p - projectionPoint;
        double distance = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
        
        // 如果距离小于阈值，则认为点击在线段上
        return distance <= 5.0;  // 5像素的点击容差
    }

    // 将点转换到箭头的局部坐标系
    QPoint center = boundingRect().center();
    QPoint localPt = pt - center;
    
    // 反向旋转点
    double cosAngle = cos(-m_rotation);
    double sinAngle = sin(-m_rotation);
    QPoint rotatedPt(
        localPt.x() * cosAngle - localPt.y() * sinAngle,
        localPt.x() * sinAngle + localPt.y() * cosAngle
    );
    
    // 将点移回原坐标系
    rotatedPt += center;
    
    // 使用未旋转的检测方法
    QPointF p1(m_line.p1());
    QPointF p2(m_line.p2());
    QPointF p(rotatedPt);
    
    // 计算线段向量
    QPointF line = p2 - p1;
    double lineLength = sqrt(line.x() * line.x() + line.y() * line.y());
    if (lineLength < 1e-6) return false;  // 线段太短
    
    // 计算点到线段的投影
    QPointF lineNormalized = line / lineLength;
    QPointF toPoint = p - p1;
    double projection = toPoint.x() * lineNormalized.x() + toPoint.y() * lineNormalized.y();
    
    // 检查投影是否在线段范围内
    if (projection < 0 || projection > lineLength) return false;
    
    // 计算点到线段的垂直距离
    QPointF projectionPoint = p1 + lineNormalized * projection;
    QPointF diff = p - projectionPoint;
    double distance = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
    
    // 如果距离小于阈值，则认为点击在线段上
    return distance <= 5.0;  // 5像素的点击容差
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

// 获取起点和终点的锚点
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

    // 添加旋转锚点
    QRect rect = boundingRect();
    int rotateSize = 12;  // 旋转锚点稍大一些
    int rotateOffset = 30;  // 距离边界的距离
    handles.push_back({
        QRect(rect.center().x() - rotateSize/2, rect.top() - rotateOffset - rotateSize/2,
              rotateSize, rotateSize),
        Handle::Rotate,
        2
    });

    return handles;
}

bool ShapeArrow::needPlusHandles() const { return false; }

void ShapeArrow::rotate(double angle)
{
    // 箭头旋转不需要特殊处理，因为基类已经处理了绘制时的旋转
    // 这里可以添加额外的旋转相关逻辑，如果需要的话
}

std::unique_ptr<ShapeBase> ShapeArrow::clone() const
{
    return std::make_unique<ShapeArrow>(m_line);
} 