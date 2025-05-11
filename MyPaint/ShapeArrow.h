#pragma once

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
    void rotate(double angle) override;                    // 实现旋转方法
    std::unique_ptr<ShapeBase> clone() const override;     // 克隆方法

    // 获取和设置线段端点的方法
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
        obj["rotation"] = m_rotation;          // 保存旋转角度
        obj["lineColor"] = m_lineColor.name(); // 保存线条颜色
        obj["lineWidth"] = m_lineWidth;        // 保存线条粗细
        return obj;
    }

    void fromJson(const QJsonObject &obj) override
    {
        QPoint p1(obj["x1"].toInt(), obj["y1"].toInt());
        QPoint p2(obj["x2"].toInt(), obj["y2"].toInt());
        m_line.setPoints(p1, p2);
        if (obj.contains("rotation"))
        {
            m_rotation = obj["rotation"].toDouble();
        }
        if (obj.contains("lineColor"))
        {
            m_lineColor = QColor(obj["lineColor"].toString());
        }
        if (obj.contains("lineWidth"))
        {
            m_lineWidth = obj["lineWidth"].toInt();
        }
    }

private:
    QLine m_line;
};
