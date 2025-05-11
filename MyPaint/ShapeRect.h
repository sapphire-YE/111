#pragma once
#include "ShapeBase.h"

class ShapeRect : public ShapeBase {
public:
  ShapeRect(const QRect &rect);
  void paintShape(QPainter *painter) override; // 只绘制矩形本身
  bool contains(const QPoint &pt) const override;
  void moveBy(const QPoint &delta) override;
  void resize(const QRect &newRect) override;
  QRect boundingRect() const override;
  std::vector<Handle> getArrowAnchors() const override;
  void rotate(double angle) override;                // 实现旋转方法
  std::unique_ptr<ShapeBase> clone() const override; // 克隆方法

  // 序列化方法
  QJsonObject toJson() const override {
    QJsonObject obj = ShapeBase::toJson();
    obj["type"] = "rect";
    obj["rotation"] = m_rotation; // 保存旋转角度
    return obj;
  }

  void fromJson(const QJsonObject &obj) override {
    ShapeBase::fromJson(obj);
    if (obj.contains("rotation")) {
      m_rotation = obj["rotation"].toDouble();
    }
  }

private:
  QRect m_rect;
};
