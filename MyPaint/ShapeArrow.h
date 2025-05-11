#ifndef SHAPEARROW_H
#define SHAPEARROW_H

#include "ShapeBase.h"
#include <QLine>

class ShapeArrow : public ShapeBase
{
public:
    ShapeArrow(const QLine &line);
    void paintShape(QPainter *painter) override;
    bool contains(const QPoint &pt) const override;
    void moveBy(const QPoint &delta) override;
    void resize(const QRect &newRect) override;
    QRect boundingRect() const override;
    std::vector<Handle> getHandles() const override;
    bool needPlusHandles() const override;
    bool isTextEditable() const override { return false; } // 箭头不支持文本编辑

    // 新增：获取和设置线段端点的方法
    const QLine &getLine() const { return m_line; }
    void setP1(const QPoint &p1) { m_line.setP1(p1); }
    void setP2(const QPoint &p2) { m_line.setP2(p2); }

    // 序列化方法
    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["type"] = "arrow";
        obj["x1"] = m_line.x1();
        obj["y1"] = m_line.y1();
        obj["x2"] = m_line.x2();
        obj["y2"] = m_line.y2();
        return obj;
    }

    void fromJson(const QJsonObject &obj) override
    {
        QPoint p1(obj["x1"].toInt(), obj["y1"].toInt());
        QPoint p2(obj["x2"].toInt(), obj["y2"].toInt());
        m_line.setPoints(p1, p2);
    }

private:
    QLine m_line;
};

#endif // SHAPEARROW_H
