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
#include <QMenu>
#include <QClipboard>

class QDragEnterEvent;
class QDropEvent;

class DrawingArea : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingArea(QWidget *parent = nullptr);
    ~DrawingArea();

    void setBackgroundColor(const QColor &color);
    void setGridSize(int size);
    void setPageSize(const QSize &size);
    void setGridVisible(bool visible); // 设置网格显示/隐藏

    // 文件操作
    bool saveToFile(const QString &fileName);
    bool loadFromFile(const QString &fileName);
    void clear(); // 清空画布

    // 导出功能
    bool exportToPNG(const QString &fileName);
    bool exportToSVG(const QString &fileName);

    // 图形排列相关方法
    void moveShapeUp();    // 上移一层
    void moveShapeDown();  // 下移一层
    void moveShapeToTop(); // 移到最顶层
    void moveShapeToBottom(); // 移到最底层

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override; // 双击事件
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override; // 拖拽进入事件
    void dropEvent(QDropEvent *event) override;           // 拖拽释放事件
    void contextMenuEvent(QContextMenuEvent *event) override; // 右键菜单事件

private:
    QColor m_bgColor = Qt::white;
    int m_gridSize = 20;                            // 默认20
    bool m_gridVisible = true;                      // 控制网格显示/隐藏
    std::vector<std::unique_ptr<ShapeBase>> shapes; // 存储所有形状的列表
    int selectedIndex = -1;                         // 当前选中的图形索引
    int snappedShapeIndex = -1;                     // 记录被吸附的图形索引
    QPoint lastMousePos;                            // 上一次鼠标位置
    bool dragging = false;                          // 是否正在拖动

    // 文本编辑相关
    QWidget *m_textEdit = nullptr;         // 改为QWidget基类指针，可以指向不同类型的文本编辑控件
    void startTextEditing(int shapeIndex); // 开始编辑文本
    void finishTextEditing();              // 完成文本编辑

    // 记录吸附的锚点信息
    struct SnapInfo
    {
        int shapeIndex = -1;
        int handleIndex = -1;
        QPoint pos;
    };
    SnapInfo snappedHandle;

    // 记录箭头和图形之间的连接关系
    struct ArrowConnection
    {
        int arrowIndex;    // 箭头的索引
        int shapeIndex;    // 连接的图形索引
        int handleIndex;   // 连接的锚点索引
        bool isStartPoint; // 是否是箭头的起点
    };
    std::vector<ArrowConnection> arrowConnections;

    // 更新连接的箭头位置
    void updateConnectedArrows(int shapeIndex, const QPoint &delta);

    // 右键菜单相关
    QMenu *m_contextMenu = nullptr;
    void createContextMenu();
    void copySelectedShape();
    void cutSelectedShape();
    void pasteShape();
    void deleteSelectedShape();
    std::unique_ptr<ShapeBase> m_clipboardShape; // 用于存储复制的图形
};

#endif // DRAWINGAREA_H