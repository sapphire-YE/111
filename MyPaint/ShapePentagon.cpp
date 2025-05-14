#include "ShapePentagon.h"
#include <QPainter>
#include <cmath>
#include <QtMath>

// 如果M_PI未定义，自行定义
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ShapePentagon::ShapePentagon(const QRect &rect) : m_rect(rect) {}

QPolygon ShapePentagon::getPolygon() const
{
    QPolygon polygon;
    QRect rect = m_rect;
    int w = rect.width();
    int h = rect.height();
    int x = rect.left();
    int y = rect.top();
    QPoint center = rect.center();
    
    // 计算五边形的五个顶点
    const int numPoints = 5;
    for (int i = 0; i < numPoints; ++i) {
        // 从顶部顶点开始，顺时针计算
        double angle = i * 2 * M_PI / numPoints - M_PI / 2; // 从顶部开始
        double px = center.x() + (w / 2.0) * cos(angle);
        double py = center.y() + (h / 2.0) * sin(angle);
        polygon << QPoint(px, py);
    }
    
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

void ShapePentagon::paintShape(QPainter *painter)
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
    
    // 绘制五边形
    painter->drawPolygon(getPolygon());
}

bool ShapePentagon::contains(const QPoint &pt) const
{
    return getPolygon().containsPoint(pt, Qt::OddEvenFill);
}

void ShapePentagon::moveBy(const QPoint &delta) 
{ 
    m_rect.translate(delta); 
}

void ShapePentagon::resize(const QRect &newRect) 
{ 
    m_rect = newRect; 
}

QRect ShapePentagon::boundingRect() const 
{ 
    return m_rect; 
}

void ShapePentagon::rotate(double angle)
{
    // 五边形旋转不需要特殊处理，因为基类已经处理了绘制时的旋转
    // 这里可以添加额外的旋转相关逻辑，如果需要的话
}

std::vector<ShapeBase::Handle> ShapePentagon::getArrowAnchors() const
{
    std::vector<Handle> anchors;
    QRect rect = m_rect;
    int w = rect.width(), h = rect.height();
    int x = rect.left(), y = rect.top();
    int size = 8;
    QPoint center = rect.center();
    
    // 计算五边形的五个顶点
    const int numPoints = 5;
    QVector<QPoint> points;
    for (int i = 0; i < numPoints; ++i) {
        double angle = i * 2 * M_PI / numPoints - M_PI / 2; // 从顶部开始
        double px = center.x() + (w / 2.0) * cos(angle);
        double py = center.y() + (h / 2.0) * sin(angle);
        points.append(QPoint(px, py));
    }
    
    // 如果有旋转，应用旋转
    if (m_rotation != 0.0) {
        QTransform transform;
        transform.translate(center.x(), center.y());
        transform.rotate(m_rotation * 180.0 / M_PI);
        transform.translate(-center.x(), -center.y());
        for (int i = 0; i < points.size(); ++i) {
            points[i] = transform.map(points[i]);
        }
    }
    
    // 在四个方向（上下左右）添加箭头锚点
    // 正确计算上下左右的中点位置，确保方向正确
    
    // 上方锚点 - 顶部顶点，垂直向上
    QPoint topAnchor = QPoint(center.x(), y);
    if (m_rotation == 0.0) {
        // 无旋转时使用顶点0
        topAnchor = QPoint(points[0].x(), points[0].y());
    }
    anchors.push_back({QRect(topAnchor.x() - size / 2, topAnchor.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 0}); // 上
    
    // 下方锚点 - 底部中点，垂直向下
    QPoint bottomAnchor = QPoint(center.x(), y + h);
    if (m_rotation == 0.0) {
        // 无旋转时使用点2和点3的中点
        bottomAnchor = QPoint((points[2].x() + points[3].x()) / 2, 
                              (points[2].y() + points[3].y()) / 2);
    }
    anchors.push_back({QRect(bottomAnchor.x() - size / 2, bottomAnchor.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 1}); // 下
    
    // 左方锚点 - 左侧中点，水平向左
    QPoint leftAnchor = QPoint(x, center.y());
    if (m_rotation == 0.0) {
        // 无旋转时使用点3和点4的中点
        leftAnchor = QPoint((points[3].x() + points[4].x()) / 2, 
                            (points[3].y() + points[4].y()) / 2);
    }
    anchors.push_back({QRect(leftAnchor.x() - size / 2, leftAnchor.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 2}); // 左
    
    // 右方锚点 - 右侧中点，水平向右
    QPoint rightAnchor = QPoint(x + w, center.y());
    if (m_rotation == 0.0) {
        // 无旋转时使用点1和点2的中点
        rightAnchor = QPoint((points[1].x() + points[2].x()) / 2, 
                             (points[1].y() + points[2].y()) / 2);
    }
    anchors.push_back({QRect(rightAnchor.x() - size / 2, rightAnchor.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 3}); // 右
    
    return anchors;
}

int ShapePentagon::mapArrowHandleToAnchor(int arrowHandleIndex) const
{
    // 对于五边形，加号锚点的direction从9开始(上下左右)，对应ArrowAnchor从0开始(上下左右)
    return arrowHandleIndex - 9;
}

std::unique_ptr<ShapeBase> ShapePentagon::clone() const
{
    auto clone = std::make_unique<ShapePentagon>(m_rect);
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