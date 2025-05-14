#include "ShapeDiamond.h"
#include <QPainter>
#include <cmath>
#include <QtMath>

// 如果M_PI未定义，自行定义
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ShapeDiamond::ShapeDiamond(const QRect &rect) : m_rect(rect) {}

QPolygon ShapeDiamond::getPolygon() const
{
    QPolygon polygon;
    QRect rect = m_rect;
    int w = rect.width();
    int h = rect.height();
    int x = rect.left();
    int y = rect.top();
    QPoint center = rect.center();
    
    // 计算菱形的四个顶点
    // 上方顶点
    polygon << QPoint(center.x(), y);
    // 右侧顶点
    polygon << QPoint(x + w, center.y());
    // 下方顶点
    polygon << QPoint(center.x(), y + h);
    // 左侧顶点
    polygon << QPoint(x, center.y());
    
    // 如果有旋转，应用旋转
    if (m_rotation != 0.0) {
        QTransform transform;
        transform.translate(center.x(), center.y());
        transform.rotate(m_rotation * 180.0 / M_PI); // 转换为角度
        transform.translate(-center.x(), -center.y());
        polygon = transform.map(polygon);
    }
    
    return polygon;
}

void ShapeDiamond::paintShape(QPainter *painter)
{
    // 根据线条类型设置不同的画笔样式
    Qt::PenStyle penStyle = Qt::SolidLine;
    switch (m_lineType)
    {
    case LineType::SolidLine:
        penStyle = Qt::SolidLine;
        break;
    case LineType::DashLine:
        penStyle = Qt::DashLine;
        break;
    case LineType::DotLine:
        penStyle = Qt::DotLine;
        break;
    }

    QPen pen(m_lineColor, m_lineWidth);
    pen.setStyle(penStyle);
    painter->setPen(pen);
    painter->setBrush(m_fillColor);
    
    // 绘制菱形
    painter->drawPolygon(getPolygon());
}

bool ShapeDiamond::contains(const QPoint &pt) const
{
    return getPolygon().containsPoint(pt, Qt::OddEvenFill);
}

void ShapeDiamond::moveBy(const QPoint &delta) 
{ 
    m_rect.translate(delta); 
}

void ShapeDiamond::resize(const QRect &newRect) 
{ 
    m_rect = newRect; 
}

QRect ShapeDiamond::boundingRect() const 
{ 
    return m_rect; 
}

void ShapeDiamond::rotate(double angle)
{
    // 菱形旋转不需要特殊处理，因为基类已经处理了绘制时的旋转
    // 这里可以添加额外的旋转相关逻辑，如果需要的话
}

std::vector<ShapeBase::Handle> ShapeDiamond::getArrowAnchors() const
{
    std::vector<Handle> anchors;
    QRect rect = m_rect;
    int w = rect.width(), h = rect.height();
    int x = rect.left(), y = rect.top();
    int size = 8;
    QPoint center = rect.center();
    
    // 获取菱形的四个顶点
    QPolygon polygon = getPolygon();
    QVector<QPoint> points;
    for (int i = 0; i < polygon.size(); ++i) {
        points.append(polygon.at(i));
    }
    
    // 在四个方向（上下左右）添加箭头锚点，直接使用菱形的四个顶点
    // 上方锚点
    QPoint topAnchor;
    if (m_rotation == 0.0) {
        topAnchor = points[0]; // 上方顶点
    } else {
        topAnchor = QPoint(center.x(), y);
    }
    anchors.push_back({QRect(topAnchor.x() - size / 2, topAnchor.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 0}); // 上
    
    // 下方锚点
    QPoint bottomAnchor;
    if (m_rotation == 0.0) {
        bottomAnchor = points[2]; // 下方顶点
    } else {
        bottomAnchor = QPoint(center.x(), y + h);
    }
    anchors.push_back({QRect(bottomAnchor.x() - size / 2, bottomAnchor.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 1}); // 下
    
    // 左方锚点
    QPoint leftAnchor;
    if (m_rotation == 0.0) {
        leftAnchor = points[3]; // 左侧顶点
    } else {
        leftAnchor = QPoint(x, center.y());
    }
    anchors.push_back({QRect(leftAnchor.x() - size / 2, leftAnchor.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 2}); // 左
    
    // 右方锚点
    QPoint rightAnchor;
    if (m_rotation == 0.0) {
        rightAnchor = points[1]; // 右侧顶点
    } else {
        rightAnchor = QPoint(x + w, center.y());
    }
    anchors.push_back({QRect(rightAnchor.x() - size / 2, rightAnchor.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 3}); // 右
    
    return anchors;
}

int ShapeDiamond::mapArrowHandleToAnchor(int arrowHandleIndex) const
{
    // 对于菱形，加号锚点的direction从9开始(上下左右)，对应ArrowAnchor从0开始(上下左右)
    return arrowHandleIndex - 9;
}

std::unique_ptr<ShapeBase> ShapeDiamond::clone() const
{
    auto clone = std::make_unique<ShapeDiamond>(m_rect);
    clone->setText(m_text);
    clone->setRotation(m_rotation);
    clone->setLineColor(m_lineColor);
    clone->setLineWidth(m_lineWidth);
    clone->setFillColor(m_fillColor);
    clone->setLineType(m_lineType);
    clone->setOpacity(m_opacity);
    clone->setFontFamily(m_font.family());
    clone->setFontSize(m_font.pointSize());
    clone->setFontBold(m_font.bold());
    clone->setFontItalic(m_font.italic());
    clone->setFontUnderline(m_font.underline());
    clone->setFontStrikeOut(m_font.strikeOut());
    clone->setTextColor(m_textColor);
    clone->setTextAlignment(m_textAlignment);
    return clone;
} 