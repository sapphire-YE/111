#pragma once
#include "ShapeBase.h"

class ShapeRect : public ShapeBase
{
public:
    ShapeRect(const QRect &rect);
    void paintShape(QPainter *painter) override; // 只绘制矩形本身
    bool contains(const QPoint &pt) const override;
    void moveBy(const QPoint &delta) override;
    void resize(const QRect &newRect) override;
    QRect boundingRect() const override;
    std::vector<Handle> getArrowAnchors() const override;

    // 序列化方法
    QJsonObject toJson() const override
    {
        QJsonObject obj = ShapeBase::toJson();
        obj["type"] = "rect";
        return obj;
    }

    void fromJson(const QJsonObject &obj) override
    {
        ShapeBase::fromJson(obj);
    }

private:
    QRect m_rect;
};
