#include "ShapeEllipse.h"
#include <QPainter>
#include <cmath>

ShapeEllipse::ShapeEllipse(const QRect &rect) : m_rect(rect) {}

void ShapeEllipse::paintShape(QPainter *painter)
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
  painter->drawEllipse(m_rect);
}

bool ShapeEllipse::contains(const QPoint &pt) const
{
  // 考虑旋转后的点击检测
  if (m_rotation == 0.0)
  {
    // 椭圆的包含判断需要考虑椭圆方程
    QPointF center = m_rect.center();
    double a = m_rect.width() / 2.0;
    double b = m_rect.height() / 2.0;
    double x = (pt.x() - center.x()) / a;
    double y = (pt.y() - center.y()) / b;
    return x * x + y * y <= 1.0;
  }

  // 将点转换到椭圆的局部坐标系
  QPoint center = m_rect.center();
  QPoint localPt = pt - center;

  // 反向旋转点
  double cosAngle = cos(-m_rotation);
  double sinAngle = sin(-m_rotation);
  QPoint rotatedPt(localPt.x() * cosAngle - localPt.y() * sinAngle,
                   localPt.x() * sinAngle + localPt.y() * cosAngle);

  // 将点移回原坐标系
  rotatedPt += center;

  // 检查点是否在未旋转的椭圆内
  QPointF centerF = m_rect.center();
  double a = m_rect.width() / 2.0;
  double b = m_rect.height() / 2.0;
  double x = (rotatedPt.x() - centerF.x()) / a;
  double y = (rotatedPt.y() - centerF.y()) / b;
  return x * x + y * y <= 1.0;
}

void ShapeEllipse::moveBy(const QPoint &delta) { m_rect.translate(delta); }

void ShapeEllipse::resize(const QRect &newRect) { m_rect = newRect; }

QRect ShapeEllipse::boundingRect() const { return m_rect; }

std::vector<ShapeBase::Handle> ShapeEllipse::getArrowAnchors() const
{
  std::vector<Handle> anchors;
  QRect rect = m_rect;
  int w = rect.width(), h = rect.height();
  int x = rect.left(), y = rect.top();
  int size = 8;
  // 上
  anchors.push_back({QRect(x + w / 2 - size / 2, y - size / 2, size, size),
                     Handle::ArrowAnchor, 0});
  // 下
  anchors.push_back({QRect(x + w / 2 - size / 2, y + h - size / 2, size, size),
                     Handle::ArrowAnchor, 1});
  // 左
  anchors.push_back({QRect(x - size / 2, y + h / 2 - size / 2, size, size),
                     Handle::ArrowAnchor, 2});
  // 右
  anchors.push_back({QRect(x + w - size / 2, y + h / 2 - size / 2, size, size),
                     Handle::ArrowAnchor, 3});
  return anchors;
}

void ShapeEllipse::rotate(double angle)
{
  // 椭圆旋转不需要特殊处理，因为基类已经处理了绘制时的旋转
  // 这里可以添加额外的旋转相关逻辑，如果需要的话
}

std::unique_ptr<ShapeBase> ShapeEllipse::clone() const
{
  auto clone = std::make_unique<ShapeEllipse>(m_rect);
  clone->setText(m_text);
  clone->setRotation(m_rotation);
  clone->setLineColor(m_lineColor);
  clone->setLineWidth(m_lineWidth);
  return clone;
}