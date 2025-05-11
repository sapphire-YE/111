#pragma once
#include <QRect>
#include <QPainter>
#include <QPoint>
#include <vector>

class ShapeArrow; // 前置声明

// 1. 先在外部声明 Handle
struct ShapeHandle {
    QRect rect;
    enum Type { Scale, Arrow, ArrowAnchor } type;
    int direction;
};

class ShapeBase {
public:
    virtual ~ShapeBase() {}
    
    // 纯虚函数，子类必须实现
    virtual void paintShape(QPainter* painter) = 0;  // 只绘制图形本身
    virtual bool contains(const QPoint& pt) const = 0;
    virtual void moveBy(const QPoint& delta) = 0;
    virtual void resize(const QRect& newRect) = 0;
    virtual QRect boundingRect() const = 0;

    // 统一用 ShapeHandle
    using Handle = ShapeHandle;

    // 在基类中实现的共同功能
    void paint(QPainter* painter, bool selected = false);
    
    // 默认实现八个缩放锚点，子类可以重写
    virtual bool needPlusHandles() const { return true; }
    virtual std::vector<Handle> getHandles() const;

    // 新增：获取箭头锚点，默认返回空
    virtual std::vector<Handle> getArrowAnchors() const { return {}; }

    // 处理锚点交互
    bool handleAnchorInteraction(const QPoint& mousePos, const QPoint& lastMousePos);
    bool isHandleSelected() const { return m_selectedHandleIndex != -1; }
    void clearHandleSelection() { m_selectedHandleIndex = -1; }
    int getSelectedHandleIndex() const { return m_selectedHandleIndex; }
    void setSelectedHandleIndex(int index) { m_selectedHandleIndex = index; }

    // 新增：文本相关方法
    virtual bool isTextEditable() const { return true; }  // 默认所有图形都可编辑文本，除了箭头
    virtual void setText(const QString& text) { m_text = text; }
    virtual QString getText() const { return m_text; }
    virtual bool isEditing() const { return m_isEditing; }
    virtual void setEditing(bool editing) { m_isEditing = editing; }

protected:
    // 计算新的矩形区域
    QRect calculateNewRect(const QPoint& mousePos, const QPoint& lastMousePos) const;

    // 新增：文本相关属性
    QString m_text;
    bool m_isEditing = false;

private:
    int m_selectedHandleIndex = -1;  // 当前选中的锚点索引
}; 