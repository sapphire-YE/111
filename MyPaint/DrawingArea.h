#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include "EllipseTextEdit.h"
#include "ShapeBase.h"
#include <QClipboard>
#include <QLineEdit>
#include <QMenu>
#include <QPoint>
#include <QRect>
#include <QWidget>
#include <memory>
#include <vector>

class QDragEnterEvent;
class QDropEvent;

class DrawingArea : public QWidget
{
  Q_OBJECT
public:
  explicit DrawingArea(QWidget *parent = nullptr);
  ~DrawingArea();

signals:
  void zoomFactorChanged(double factor);    // 当缩放因子变化时发出信号
  void shapeSelected(ShapeBase *shape);     // 当选中图形时发出信号
  void selectionCleared();                  // 当取消选择时发出信号
  void backgroundColorChanged(QColor color); // 当背景颜色变化时发出信号
  void pageSizeChanged(QSize size);         // 当页面大小变化时发出信号
  void gridSizeChanged(int size);           // 当网格大小变化时发出信号
  void gridVisibilityChanged(bool visible); // 当网格显示状态变化时发出信号

public:
  void setBackgroundColor(const QColor &color)
  {
    if (m_bgColor != color) {
      m_bgColor = color;
      emit backgroundColorChanged(color);
      update();
    }
  }
  QColor getBackgroundColor() const { return m_bgColor; }
  void setGridSize(int size);
  void setPageSize(const QSize &size);
  void setGridVisible(bool visible); // 设置网格显示/隐藏

  // 缩放功能
  void setZoomFactor(double factor);                    // 设置缩放因子
  void zoomIn();                                        // 放大
  void zoomOut();                                       // 缩小
  void resetZoom();                                     // 重置缩放
  double getZoomFactor() const { return m_zoomFactor; } // 获取当前缩放因子

  // 文件操作
  bool saveToFile(const QString &fileName);
  bool loadFromFile(const QString &fileName);
  void clear(); // 清空画布

  // 导出功能
  bool exportToPNG(const QString &fileName);
  bool exportToSVG(const QString &fileName);

  // 图形排列相关方法
  void moveShapeUp();       // 上移一层
  void moveShapeDown();     // 下移一层
  void moveShapeToTop();    // 移到最顶层
  void moveShapeToBottom(); // 移到最底层

  // 设置选中图形的线条颜色和粗细
  void setSelectedShapeLineColor(const QColor &color);
  void setSelectedShapeLineWidth(int width);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override; // 双击事件
  void keyPressEvent(QKeyEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;     // 拖拽进入事件
  void dropEvent(QDropEvent *event) override;               // 拖拽释放事件
  void contextMenuEvent(QContextMenuEvent *event) override; // 右键菜单事件
  void wheelEvent(QWheelEvent *event) override;             // 滚轮事件用于缩放支持

private:
  QColor m_bgColor = Qt::white;
  int m_gridSize = 20;                            // 默认20
  bool m_gridVisible = true;                      // 控制网格显示/隐藏
  double m_zoomFactor = 1.0;                      // 缩放因子
  QSize m_pageSize = QSize(1050, 1500);           // 默认页面大小(A4)
  std::vector<std::unique_ptr<ShapeBase>> shapes; // 存储所有形状的列表
  int selectedIndex = -1;                         // 当前选中的图形索引
  int snappedShapeIndex = -1;                     // 记录被吸附的图形索引
  QPoint lastMousePos;                            // 上一次鼠标位置
  bool dragging = false;                          // 是否正在拖动

  // 坐标转换函数（考虑缩放因子）
  QPoint screenToDoc(const QPoint &pos) const; // 屏幕坐标转文档坐标
  QPoint docToScreen(const QPoint &pos) const; // 文档坐标转屏幕坐标
  QRect screenToDoc(const QRect &rect) const;  // 屏幕矩形转文档矩形
  QRect docToScreen(const QRect &rect) const;  // 文档矩形转屏幕矩形
  QSize docToScreen(const QSize &size) const;  // 文档大小转屏幕大小

  // 文本编辑相关
  QWidget *m_textEdit =
      nullptr;                           // 改为QWidget基类指针，可以指向不同类型的文本编辑控件
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