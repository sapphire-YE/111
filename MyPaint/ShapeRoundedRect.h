#pragma once
#include "ShapeBase.h"

class ShapeRoundedRect : public ShapeBase
{
public:
  ShapeRoundedRect(const QRect &rect, qreal xRadius = 10, qreal yRadius = 10);
  void paintShape(QPainter *painter) override;
  bool contains(const QPoint &pt) const override;
  void moveBy(const QPoint &delta) override;
  void resize(const QRect &newRect) override;
  QRect boundingRect() const override;
  std::vector<Handle> getArrowAnchors() const override;
  int mapArrowHandleToAnchor(int arrowHandleIndex) const override;
  void rotate(double angle) override;
  std::unique_ptr<ShapeBase> clone() const override;

  // 圆角半径设置和获取
  void setRadiusX(qreal radius) { m_xRadius = radius; }
  void setRadiusY(qreal radius) { m_yRadius = radius; }
  qreal radiusX() const { return m_xRadius; }
  qreal radiusY() const { return m_yRadius; }

  // 序列化方法
  QJsonObject toJson() const override
  {
    QJsonObject obj = ShapeBase::toJson();
    obj["type"] = "roundedrect";
    obj["rotation"] = m_rotation;
    obj["xRadius"] = m_xRadius;
    obj["yRadius"] = m_yRadius;
    return obj;
  }

  void fromJson(const QJsonObject &obj) override
  {
    ShapeBase::fromJson(obj);
    if (obj.contains("rotation"))
      m_rotation = obj["rotation"].toDouble();
    if (obj.contains("xRadius"))
      m_xRadius = obj["xRadius"].toDouble();
    if (obj.contains("yRadius"))
      m_yRadius = obj["yRadius"].toDouble();
  }

private:
  QRect m_rect;
  qreal m_xRadius;  // x方向圆角半径
  qreal m_yRadius;  // y方向圆角半径
};
