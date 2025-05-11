#include "ShapePolygon.h"
#include <QPainter>
#include <cmath>

ShapePolygon::ShapePolygon(const QPolygon &polygon) : m_polygon(polygon) {}

void ShapePolygon::paintShape(QPainter *painter)
{
  if (!painter)
    return;
  painter->drawPolygon(m_polygon);
}

bool ShapePolygon::contains(const QPoint &pt) const
{
  // 考虑旋转后的点击检测
  if (m_rotation == 0.0)
  {
    return m_polygon.containsPoint(pt, Qt::OddEvenFill);
  }

  // 将点转换到多边形的局部坐标系
  QPoint center = boundingRect().center();
  QPoint localPt = pt - center;

  // 反向旋转点
  double cosAngle = cos(-m_rotation);
  double sinAngle = sin(-m_rotation);
  QPoint rotatedPt(localPt.x() * cosAngle - localPt.y() * sinAngle,
                   localPt.x() * sinAngle + localPt.y() * cosAngle);

  // 将点移回原坐标系
  rotatedPt += center;

  // 检查点是否在未旋转的多边形内
  return m_polygon.containsPoint(rotatedPt, Qt::OddEvenFill);
}

void ShapePolygon::moveBy(const QPoint &delta) { m_polygon.translate(delta); }

void ShapePolygon::resize(const QRect &newRect)
{

  // 这里使用简单的实现，将多边形缩放到新矩形
  QRect oldRect = boundingRect();
  if (oldRect.isEmpty())
    return;

  QPolygon newPolygon;
  for (const QPoint &pt : m_polygon)
  {
    int newX = newRect.left() +
               (pt.x() - oldRect.left()) * newRect.width() / oldRect.width();
    int newY = newRect.top() +
               (pt.y() - oldRect.top()) * newRect.height() / oldRect.height();
    newPolygon << QPoint(newX, newY);
  }
  m_polygon = newPolygon;
}

QRect ShapePolygon::boundingRect() const { return m_polygon.boundingRect(); }

void ShapePolygon::rotate(double angle)
{
  // 更新旋转角度
  m_rotation = angle;

  // 计算旋转中心点
  QPoint center = boundingRect().center();

  // 创建变换矩阵
  QTransform transform;
  transform.translate(center.x(), center.y());
  transform.rotate(angle);
  transform.translate(-center.x(), -center.y());

  // 应用旋转变换到多边形
  m_polygon = transform.map(m_polygon);
}

std::unique_ptr<ShapeBase> ShapePolygon::clone() const
{
  return std::make_unique<ShapePolygon>(m_polygon);
}