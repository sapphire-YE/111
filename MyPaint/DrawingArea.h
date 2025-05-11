#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QWidget>
#include <vector>
#include <QPoint>
#include <QRect>
#include <memory>
#include "ShapeBase.h"
#include <QLineEdit>
#include "EllipseTextEdit.h"

class QDragEnterEvent;
class QDropEvent;

class DrawingArea : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingArea(QWidget *parent = nullptr);

    // 新增
    void setBackgroundColor(const QColor& color);
    void setGridSize(int size); // 新增
    void setPageSize(const QSize& size); // 新增

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;  // 新增：双击事件
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override; // 拖拽进入事件
    void dropEvent(QDropEvent *event) override; // 拖拽释放事件

private:
    QColor m_bgColor = Qt::white; // 新增
    int m_gridSize = 20; // 新增，默认20
    std::vector<std::unique_ptr<ShapeBase>> shapes; // 存储所有形状的列表
    int selectedIndex = -1;   // 当前选中的图形索引
    int snappedShapeIndex = -1;  // 新增：记录被吸附的图形索引
    QPoint lastMousePos;      // 上一次鼠标位置
    bool dragging = false;    // 是否正在拖动

    // 修改：文本编辑相关
    QWidget* m_textEdit = nullptr;  // 改为QWidget基类指针，可以指向不同类型的文本编辑控件
    void startTextEditing(int shapeIndex);  // 开始编辑文本
    void finishTextEditing();  // 完成文本编辑

    // 新增：记录吸附的锚点信息
    struct SnapInfo {
        int shapeIndex = -1;
        int handleIndex = -1;
        QPoint pos;
    };
    SnapInfo snappedHandle;

    // 新增：记录箭头和图形之间的连接关系
    struct ArrowConnection {
        int arrowIndex;           // 箭头的索引
        int shapeIndex;           // 连接的图形索引
        int handleIndex;          // 连接的锚点索引
        bool isStartPoint;        // 是否是箭头的起点
    };
    std::vector<ArrowConnection> arrowConnections;

    // 新增：更新连接的箭头位置
    void updateConnectedArrows(int shapeIndex, const QPoint& delta);
};

#endif // DRAWINGAREA_H 