#include "ShapeRect.h"
#include <QPainter>
#include <cmath>

ShapeRect::ShapeRect(const QRect &rect) : m_rect(rect) {}

void ShapeRect::paintShape(QPainter *painter)
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
  painter->drawRect(m_rect);
}

bool ShapeRect::contains(const QPoint &pt) const
{
  // 考虑旋转后的点击检测
  if (m_rotation == 0.0)
  {
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

void ShapeRect::rotate(double angle)
{
  // 矩形旋转不需要特殊处理，因为基类已经处理了绘制时的旋转
  // 这里可以添加额外的旋转相关逻辑，如果需要的话
}

std::vector<ShapeBase::Handle> ShapeRect::getArrowAnchors() const
{
  std::vector<Handle> anchors;
  QRect rect = m_rect;
  int w = rect.width(), h = rect.height();
  int x = rect.left(), y = rect.top();
  int size = 8;
  QPoint center = rect.center();

  if (m_rotation != 0.0) {
    // 计算旋转后的锚点位置
    auto rotatePoint = [&](QPoint pt) {
      QPoint localPt = pt - center;
      double cosAngle = cos(m_rotation);
      double sinAngle = sin(m_rotation);
      QPoint rotatedPt(
        localPt.x() * cosAngle - localPt.y() * sinAngle,
        localPt.x() * sinAngle + localPt.y() * cosAngle
      );
      return rotatedPt + center;
    };

    // 计算各个箭头锚点的旋转后位置
    QPoint topMiddle = rotatePoint(QPoint(x + w / 2, y));
    QPoint bottomMiddle = rotatePoint(QPoint(x + w / 2, y + h));
    QPoint leftMiddle = rotatePoint(QPoint(x, y + h / 2));
    QPoint rightMiddle = rotatePoint(QPoint(x + w, y + h / 2));

    // 添加旋转后的箭头锚点
    anchors.push_back({QRect(topMiddle.x() - size / 2, topMiddle.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 0}); // 上
    anchors.push_back({QRect(bottomMiddle.x() - size / 2, bottomMiddle.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 1}); // 下
    anchors.push_back({QRect(leftMiddle.x() - size / 2, leftMiddle.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 2}); // 左
    anchors.push_back({QRect(rightMiddle.x() - size / 2, rightMiddle.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 3}); // 右
  } else {
    // 无旋转时的原始实现
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
  }
  return anchors;
}

int ShapeRect::mapArrowHandleToAnchor(int arrowHandleIndex) const
{
  // 对于矩形，加号锚点的direction从9开始(上下左右)，对应ArrowAnchor从0开始(上下左右)
  return arrowHandleIndex - 9;
}

std::unique_ptr<ShapeBase> ShapeRect::clone() const
{
  auto clone = std::make_unique<ShapeRect>(m_rect);
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