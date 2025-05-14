#include "ShapeLibraryWidget.h"
#include "DrawingArea.h"
#include <QDrag>
#include <QMimeData>
#include <QDebug>

ShapeLibraryWidget::ShapeLibraryWidget(QWidget *parent)
    : QWidget(parent), m_listWidget(new QListWidget(this)),
      m_layout(new QVBoxLayout(this)), m_drawingArea(nullptr)
{
  m_layout->addWidget(m_listWidget);
  m_layout->setContentsMargins(0, 0, 0, 0);
  setLayout(m_layout);

  // 设置列表控件属性
  m_listWidget->setDragEnabled(true);
  m_listWidget->setViewMode(QListWidget::IconMode);
  m_listWidget->setIconSize(QSize(40, 40)); // 调整图标大小
  m_listWidget->setSpacing(8);              // 减小间距
  m_listWidget->setAcceptDrops(false);
  m_listWidget->setDropIndicatorShown(false);
  // 隐藏文本，只显示图标
  m_listWidget->setTextElideMode(Qt::ElideNone);
  m_listWidget->setGridSize(QSize(50, 50));         // 调整网格尺寸
  m_listWidget->setResizeMode(QListWidget::Adjust); // 调整大小模式
  m_listWidget->setMovement(QListWidget::Static);   // 静态项目，不可移动
  m_listWidget->setFlow(QListWidget::LeftToRight);  // 从左到右流动

  // 启用工具提示
  enableToolTips();

  // 连接拖拽信号
  connect(m_listWidget, &QListWidget::itemPressed, this,
          [this](QListWidgetItem *item)
          {
            if (item)
            {
              startDrag(item);
            }
          });

  // 初始化图形项
  initShapeItems();
}

ShapeLibraryWidget::~ShapeLibraryWidget() {}

void ShapeLibraryWidget::initShapeItems()
{
  // 添加基本图形
  addShapeItem(tr("Rectangle"), "rect");
  addShapeItem(tr("Ellipse"), "ellipse");
  addShapeItem(tr("Triangle"), "triangle");
  addShapeItem(tr("Diamond"), "diamond");
  addShapeItem(tr("Pentagon"), "pentagon");
  // addShapeItem("Polygon", "polygon");
  addShapeItem(tr("Arrow"), "arrow");
}

QString ShapeLibraryWidget::getCurrentShapeType() const
{
  QListWidgetItem *currentItem = m_listWidget->currentItem();
  return currentItem ? currentItem->data(Qt::UserRole).toString() : QString();
}

void ShapeLibraryWidget::startDrag(const QListWidgetItem *item)
{
  if (!item)
    return;

  QString shapeType = item->data(Qt::UserRole).toString();

  QMimeData *mimeData = new QMimeData;
  mimeData->setData("application/x-shape-type", shapeType.toUtf8());

  QDrag *drag = new QDrag(this);
  drag->setMimeData(mimeData);

  // 设置拖拽时的预览图标
  QIcon icon = item->icon();
  if (!icon.isNull())
  {
    QPixmap pixmap = icon.pixmap(48, 48);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
  }

  drag->exec(Qt::CopyAction);
}

void ShapeLibraryWidget::addShapeItem(const QString &name,
                                      const QString &type)
{
  QListWidgetItem *item = new QListWidgetItem(""); // 使用空文本，完全依赖工具提示
  item->setData(Qt::UserRole, type);
  item->setData(Qt::DisplayRole, name); // 保存显示名称为数据

  // 尝试不同的图标路径
  QString iconPath = ":/" + (QString("icons/") + type) + ".png";
  QIcon icon(iconPath);
  qDebug() << "Try to load icon:" << iconPath << (icon.isNull() ? "Failed" : "Success");

  // 如果图标不为空，则设置
  item->setIcon(icon);

  // 确保图标足够大
  QSize iconSize = QSize(40, 40);
  QPixmap pixmap = icon.pixmap(iconSize);
  qDebug() << "IconSize:" << pixmap.width() << "x" << pixmap.height() << "Empty or not:" << pixmap.isNull();

  // 设置增强的提示文本
  QString tooltipHtml = QString("<div style='text-align: center;'>"
                                "<b>%1</b>"
                                "<br><span style='color: #666;'>%2</span>"
                                "</div>")
                            .arg(name)
                            .arg(tr("Click to select, drag to use"));
  item->setToolTip(tooltipHtml);

  // 设置足够的大小以显示图标
  item->setSizeHint(QSize(50, 50));

  // 设置居中对齐
  item->setTextAlignment(Qt::AlignCenter);

  m_listWidget->addItem(item);
}

void ShapeLibraryWidget::enableToolTips()
{
  m_listWidget->setToolTip("");
  m_listWidget->viewport()->setAttribute(Qt::WA_AlwaysShowToolTips, true);

  // 确保文本完全隐藏，只在悬停时显示作为提示
  // 添加更美观的视觉效果
  m_listWidget->setStyleSheet(
      "QListWidget { background-color: #f0f0f0; border: 1px solid #cccccc; }"       // 列表背景
      "QListWidget::item { color: transparent; border-radius: 4px; padding: 3px; }" // 使文本透明，项目圆角
      "QListWidget::item:hover { background-color: rgba(0, 120, 215, 40); }"        // 悬停效果
      "QListWidget::item:selected { background-color: rgba(0, 120, 215, 80); }"     // 选中项的背景色
  );
}

void ShapeLibraryWidget::setDrawingArea(DrawingArea *drawingArea)
{
  m_drawingArea = drawingArea;
}
