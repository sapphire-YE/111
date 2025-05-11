#pragma once
#include <QRect>
#include <QPainter>
#include <QPoint>
#include <vector>
#include <QJsonObject>
#include <memory>
#include <QColor>

class ShapeArrow; // 前置声明

// 1. 先在外部声明 Handle
struct ShapeHandle
{
    QRect rect;
    enum Type
    {
        Scale,
        Arrow,
        ArrowAnchor,
        Rotate  // 旋转锚点类型
    } type;
    int direction;
};

class ShapeBase
{
public:
    virtual ~ShapeBase() {}

    // 纯虚函数，子类必须实现
    virtual void paintShape(QPainter *painter) = 0; // 只绘制图形本身
    virtual bool contains(const QPoint &pt) const = 0;
    virtual void moveBy(const QPoint &delta) = 0;
    virtual void resize(const QRect &newRect) = 0;
    virtual QRect boundingRect() const = 0;
    virtual void rotate(double angle) = 0;  // 旋转图形

    // 复制和矩形操作相关方法
    virtual std::unique_ptr<ShapeBase> clone() const = 0;  // 克隆当前图形
    virtual QRect getRect() const { return boundingRect(); }  // 获取矩形区域
    virtual void setRect(const QRect &rect) { resize(rect); }  // 设置矩形区域

    // 统一用 ShapeHandle
    using Handle = ShapeHandle;

    // 在基类中实现的共同功能
    void paint(QPainter *painter, bool selected = false);

    // 默认实现八个缩放锚点，子类可以重写
    virtual bool needPlusHandles() const { return true; }
    virtual std::vector<Handle> getHandles() const;

    // 获取箭头锚点，默认返回空
    virtual std::vector<Handle> getArrowAnchors() const { return {}; }

    // 旋转相关方法
    virtual double getRotation() const { return m_rotation; }  // 获取当前旋转角度
    virtual void setRotation(double angle) { m_rotation = angle; }  // 设置旋转角度

    // 处理锚点交互
    bool handleAnchorInteraction(const QPoint &mousePos, const QPoint &lastMousePos);
    bool isHandleSelected() const { return m_selectedHandleIndex != -1; }
    void clearHandleSelection() { m_selectedHandleIndex = -1; }
    int getSelectedHandleIndex() const { return m_selectedHandleIndex; }
    void setSelectedHandleIndex(int index) { m_selectedHandleIndex = index; }

    // 文本相关方法
    virtual bool isTextEditable() const { return true; } // 默认所有图形都可编辑文本，除了箭头
    virtual void setText(const QString &text) { m_text = text; }
    virtual QString getText() const { return m_text; }
    virtual bool isEditing() const { return m_isEditing; }
    virtual void setEditing(bool editing) { m_isEditing = editing; }

    // 序列化相关方法
    virtual QJsonObject toJson() const
    {
        QJsonObject obj;
        QRect rect = boundingRect();
        obj["x"] = rect.x();
        obj["y"] = rect.y();
        obj["width"] = rect.width();
        obj["height"] = rect.height();
        obj["text"] = m_text;
        return obj;
    }

    virtual void fromJson(const QJsonObject &obj)
    {
        QRect rect(
            obj["x"].toInt(),
            obj["y"].toInt(),
            obj["width"].toInt(),
            obj["height"].toInt());
        resize(rect);
        if (obj.contains("text"))
        {
            m_text = obj["text"].toString();
        }
        if (obj.contains("lineColor")) {
            m_lineColor = QColor(obj["lineColor"].toString());
        }
        if (obj.contains("lineWidth")) {
            m_lineWidth = obj["lineWidth"].toInt();
        }
    }

    // 线条样式相关方法
    virtual void setLineColor(const QColor &color) { m_lineColor = color; }
    virtual QColor getLineColor() const { return m_lineColor; }
    virtual void setLineWidth(int width) { m_lineWidth = width; }
    virtual int getLineWidth() const { return m_lineWidth; }

protected:
    // 计算新的矩形区域
    QRect calculateNewRect(const QPoint &mousePos, const QPoint &lastMousePos) const;

    // 文本相关属性
    QString m_text;
    bool m_isEditing = false;
    double m_rotation = 0.0;  // 旋转角度（弧度）
    QColor m_lineColor = Qt::black;  // 线条颜色
    int m_lineWidth = 1;  // 线条粗细

private:
    int m_selectedHandleIndex = -1; // 当前选中的锚点索引
};
