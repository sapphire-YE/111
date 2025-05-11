#include "ShapeLibraryWidget.h"
#include <QDrag>
#include <QMimeData>

ShapeLibraryWidget::ShapeLibraryWidget(QWidget *parent)
    : QWidget(parent)
    , m_listWidget(new QListWidget(this))
    , m_layout(new QVBoxLayout(this))
{
    m_layout->addWidget(m_listWidget);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);

    // 设置列表控件属性
    m_listWidget->setDragEnabled(true);
    m_listWidget->setViewMode(QListWidget::IconMode);
    m_listWidget->setIconSize(QSize(32, 32));
    m_listWidget->setSpacing(10);
    m_listWidget->setAcceptDrops(false);
    m_listWidget->setDropIndicatorShown(false);

    // 连接拖拽信号
    connect(m_listWidget, &QListWidget::itemPressed, this, [this](QListWidgetItem* item) {
        if (item) {
            startDrag(item);
        }
    });

    // 初始化图形项
    initShapeItems();
}

ShapeLibraryWidget::~ShapeLibraryWidget()
{
}

void ShapeLibraryWidget::initShapeItems()
{
    // 添加基本图形
    addShapeItem(tr("Rectangle"), "rect");
    addShapeItem(tr("Ellipse"), "ellipse");
    // addShapeItem(tr("Polygon"), "polygon");
    addShapeItem(tr("Arrow"), "arrow");
}

QString ShapeLibraryWidget::getCurrentShapeType() const
{
    QListWidgetItem* currentItem = m_listWidget->currentItem();
    return currentItem ? currentItem->data(Qt::UserRole).toString() : QString();
}

void ShapeLibraryWidget::startDrag(const QListWidgetItem* item)
{
    if (!item) return;

    QString shapeType = item->data(Qt::UserRole).toString();
    
    QMimeData* mimeData = new QMimeData;
    mimeData->setData("application/x-shape-type", shapeType.toUtf8());

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);

    // 可以设置拖拽时的预览图标
    // drag->setPixmap(...);

    drag->exec(Qt::CopyAction);
}

void ShapeLibraryWidget::addShapeItem(const QString& name, const QString& type)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setData(Qt::UserRole, type);
    // 可以设置图标
    // item->setIcon(QIcon(":/icons/" + type + ".png"));
    m_listWidget->addItem(item);
}
