#ifndef SHAPELIBRARYWIDGET_H
#define SHAPELIBRARYWIDGET_H

#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>

class ShapeLibraryWidget : public QWidget
{
  Q_OBJECT
public:
  explicit ShapeLibraryWidget(QWidget *parent = nullptr);
  ~ShapeLibraryWidget();

  // 初始化图形库项目
  void initShapeItems();
  // 获取当前选中的图形类型
  QString getCurrentShapeType() const;
  // 启用工具提示
  void enableToolTips();
  // 设置绘图区域
  void setDrawingArea(class DrawingArea* drawingArea);

protected:
  // 重写拖拽事件
  void startDrag(const QListWidgetItem *item);

private:
  QListWidget *m_listWidget;
  QVBoxLayout *m_layout;
  class DrawingArea *m_drawingArea;

  // 添加一个图形项到列表
  void addShapeItem(const QString &name, const QString &type);
};

#endif // SHAPELIBRARYWIDGET_H
