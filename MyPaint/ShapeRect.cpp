#include "ShapeRect.h"
#include <QPainter>
#include <cmath>

ShapeRect::ShapeRect(const QRect &rect) : m_rect(rect) {}

void ShapeRect::paintShape(QPainter *painter) {
  painter->setPen(QPen(Qt::black, 1));
  painter->setBrush(Qt::white);
  painter->drawRect(m_rect);
}

bool ShapeRect::contains(const QPoint &pt) const {
  // 考虑旋转后的点击检测
  if (m_rotation == 0.0) {
    return m_rect.contains(pt);
  }

  // 将点转换到矩形的局部坐标系
  QPoint center = m_rect.center();
  QPoint localPt = pt - center;

  // 反向旋转点
  double cosAngle = cos(-m_rotation);
  double sinAngle = sin(-m_rotation);
  QPoint rotatedPt(localPt.x() * cosAngle - localPt.y() * sinAngle,
                   localPt.x() * sinAngle + localPt.y() * cosAngle);

  // 将点移回原坐标系
  rotatedPt += center;

  // 检查点是否在未旋转的矩形内
  return m_rect.contains(rotatedPt);
}

void ShapeRect::moveBy(const QPoint &delta) { m_rect.translate(delta); }

void ShapeRect::resize(const QRect &newRect) { m_rect = newRect; }

QRect ShapeRect::boundingRect() const { return m_rect; }

void ShapeRect::rotate(double angle) {
  // 矩形旋转不需要特殊处理，因为基类已经处理了绘制时的旋转
  // 这里可以添加额外的旋转相关逻辑，如果需要的话
}

std::vector<ShapeBase::Handle> ShapeRect::getArrowAnchors() const {
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

std::unique_ptr<ShapeBase> ShapeRect::clone() const {
  return std::make_unique<ShapeRect>(m_rect);
}