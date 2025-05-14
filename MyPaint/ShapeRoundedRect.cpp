#include "ShapeRoundedRect.h"
#include <cmath>

ShapeRoundedRect::ShapeRoundedRect(const QRect &rect, qreal xRadius, qreal yRadius)
    : m_rect(rect), m_xRadius(xRadius), m_yRadius(yRadius) {}

void ShapeRoundedRect::paintShape(QPainter *painter)
{
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
  
  // 使用drawRoundedRect代替drawRect
  painter->drawRoundedRect(m_rect, m_xRadius, m_yRadius);
}

bool ShapeRoundedRect::contains(const QPoint &pt) const
{
  if (m_rotation == 0.0)
  {
    // 对于圆角矩形，我们仍然使用普通矩形的包含检测
    // 这是一个简化的实现，实际上应该考虑圆角部分
    return m_rect.contains(pt);
  }

  QPoint center = m_rect.center();
  QPoint localPt = pt - center;

  double cosAngle = cos(-m_rotation);
  double sinAngle = sin(-m_rotation);
  QPoint rotatedPt(localPt.x() * cosAngle - localPt.y() * sinAngle,
                   localPt.x() * sinAngle + localPt.y() * cosAngle);

  rotatedPt += center;

  return m_rect.contains(rotatedPt);
}

void ShapeRoundedRect::moveBy(const QPoint &delta)
{
  m_rect.translate(delta);
}

void ShapeRoundedRect::resize(const QRect &newRect)
{
  m_rect = newRect;
}

QRect ShapeRoundedRect::boundingRect() const
{
  return m_rect;
}

std::vector<ShapeBase::Handle> ShapeRoundedRect::getArrowAnchors() const
{
  std::vector<Handle> anchors;
  QRect rect = m_rect;
  int w = rect.width(), h = rect.height();
  int x = rect.left(), y = rect.top();
  int size = 8;
  QPoint center = rect.center();

  if (m_rotation != 0.0) {
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

    QPoint topMiddle = rotatePoint(QPoint(x + w / 2, y));
    QPoint bottomMiddle = rotatePoint(QPoint(x + w / 2, y + h));
    QPoint leftMiddle = rotatePoint(QPoint(x, y + h / 2));
    QPoint rightMiddle = rotatePoint(QPoint(x + w, y + h / 2));

    anchors.push_back({QRect(topMiddle.x() - size / 2, topMiddle.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 0});
    anchors.push_back({QRect(bottomMiddle.x() - size / 2, bottomMiddle.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 1});
    anchors.push_back({QRect(leftMiddle.x() - size / 2, leftMiddle.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 2});
    anchors.push_back({QRect(rightMiddle.x() - size / 2, rightMiddle.y() - size / 2, size, size),
                       Handle::ArrowAnchor, 3});
  } else {
    anchors.push_back({QRect(x + w / 2 - size / 2, y - size / 2, size, size),
                      Handle::ArrowAnchor, 0});
    anchors.push_back({QRect(x + w / 2 - size / 2, y + h - size / 2, size, size),
                      Handle::ArrowAnchor, 1});
    anchors.push_back({QRect(x - size / 2, y + h / 2 - size / 2, size, size),
                      Handle::ArrowAnchor, 2});
    anchors.push_back({QRect(x + w - size / 2, y + h / 2 - size / 2, size, size),
                      Handle::ArrowAnchor, 3});
  }
  return anchors;
}

int ShapeRoundedRect::mapArrowHandleToAnchor(int arrowHandleIndex) const
{
  return arrowHandleIndex - 9;
}

void ShapeRoundedRect::rotate(double angle)
{
  // 基类已经处理了绘制时的旋转
  // 这里可以添加额外的旋转相关逻辑，如果需要的话
}

std::unique_ptr<ShapeBase> ShapeRoundedRect::clone() const
{
  auto clone = std::make_unique<ShapeRoundedRect>(m_rect, m_xRadius, m_yRadius);
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
