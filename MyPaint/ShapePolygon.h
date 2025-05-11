#ifndef SHAPEPOLYGON_H
#define SHAPEPOLYGON_H

#include "ShapeBase.h"
#include <QPolygon>

class ShapePolygon : public ShapeBase
{
public:
  ShapePolygon(const QPolygon &polygon);
  void paintShape(QPainter *painter) override;
  bool contains(const QPoint &pt) const override;
  void moveBy(const QPoint &delta) override;
  void resize(const QRect &newRect) override;
  QRect boundingRect() const override;
  void rotate(double angle) override;
  std::unique_ptr<ShapeBase> clone() const override;

  // 序列化方法
  QJsonObject toJson() const override
  {
    QJsonObject obj = ShapeBase::toJson();
    obj["type"] = "polygon";
    obj["rotation"] = m_rotation; // 保存旋转角度
    return obj;
  }

  void fromJson(const QJsonObject &obj) override
  {
    ShapeBase::fromJson(obj);
    if (obj.contains("rotation"))
    {
      m_rotation = obj["rotation"].toDouble();
    }
  }

private:
  QPolygon m_polygon;
};

#endif // SHAPEPOLYGON_H