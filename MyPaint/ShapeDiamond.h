#pragma once
#include "ShapeBase.h"
#include <QPolygon>

class ShapeDiamond : public ShapeBase
{
public:
    ShapeDiamond(const QRect &rect);
    void paintShape(QPainter *painter) override; // 绘制菱形本身
    bool contains(const QPoint &pt) const override;
    void moveBy(const QPoint &delta) override;
    void resize(const QRect &newRect) override;
    QRect boundingRect() const override;
    std::vector<Handle> getArrowAnchors() const override;
    int mapArrowHandleToAnchor(int arrowHandleIndex) const override;
    void rotate(double angle) override;                // 实现旋转方法
    std::unique_ptr<ShapeBase> clone() const override; // 克隆方法

    // 序列化方法
    QJsonObject toJson() const override
    {
        QJsonObject obj = ShapeBase::toJson();
        obj["type"] = "diamond";
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
    QRect m_rect;
    QPolygon getPolygon() const; // 计算菱形的多边形点
};