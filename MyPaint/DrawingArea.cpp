#include "DrawingArea.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMimeData>
#include "ShapeFactory.h"
#include <algorithm>
#include <QLineEdit>

DrawingArea::DrawingArea(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("drawingArea");
    setFocusPolicy(Qt::StrongFocus); // 允许接收键盘事件
    setAcceptDrops(true); // 允许接收拖拽
}

void DrawingArea::setBackgroundColor(const QColor& color)
{
    m_bgColor = color;
    update();
}

void DrawingArea::setGridSize(int size)
{
    m_gridSize = size;
    update();
}

void DrawingArea::setPageSize(const QSize& size)
{
    setFixedSize(size);
}

void DrawingArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), m_bgColor); // 先填充背景色

    painter.setRenderHint(QPainter::Antialiasing);  // 抗锯齿

    // 画网格
    int gridSize = m_gridSize; // 网格间距
    int majorGridStep = 5; // 每5格一条粗线
    QPen thinPen(QColor(200,200,200), 1); // 细线浅灰色
    QPen thickPen(QColor(120,120,120), 2); // 粗线深灰色
    // 竖线
    for (int x = 0, idx = 0; x < width(); x += gridSize, ++idx) {
        if (idx % majorGridStep == 0) {
            painter.setPen(thickPen);
        } else {
            painter.setPen(thinPen);
        }
        painter.drawLine(x, 0, x, height());
    }
    // 横线
    for (int y = 0, idx = 0; y < height(); y += gridSize, ++idx) {
        if (idx % majorGridStep == 0) {
            painter.setPen(thickPen);
        } else {
            painter.setPen(thinPen);
        }
        painter.drawLine(0, y, width(), y);
    }

    // 画图形
    for (int i = 0; i < shapes.size(); ++i) {
        bool showHandles = false;
        if (i == snappedHandle.shapeIndex) showHandles = true;
        if (i == selectedIndex) {
            auto* arrow = dynamic_cast<ShapeArrow*>(shapes[i].get());
            if (arrow && shapes[i]->isHandleSelected()) showHandles = false;
            else showHandles = true;
        }
        shapes[i]->paint(&painter, showHandles);
    }

    QWidget::paintEvent(event); // 调用父类paintEvent
}

void DrawingArea::mousePressEvent(QMouseEvent *event)
{
    if (selectedIndex != -1) {
        // 检查是否点击了锚点
        const auto& handles = shapes[selectedIndex]->getHandles();
        for (size_t i = 0; i < handles.size(); ++i) {
            if (handles[i].rect.contains(event->pos())) {
                shapes[selectedIndex]->setSelectedHandleIndex(i);
                lastMousePos = event->pos();
                dragging = true;
                update();
                return;
            }
        }
    }

    selectedIndex = -1;
    for (int i = shapes.size() - 1; i >= 0; --i) {
        if (shapes[i]->boundingRect().contains(event->pos())) {
            selectedIndex = i;
            lastMousePos = event->pos();
            dragging = true;
            update();
            return;
        }
    }
    update();
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging && selectedIndex != -1) {
        if (shapes[selectedIndex]->isHandleSelected()) {
            if (auto* arrow = dynamic_cast<ShapeArrow*>(shapes[selectedIndex].get())) {
                int handleIndex = arrow->getSelectedHandleIndex();
                if (handleIndex != -1) {
                    QPoint mousePos = event->pos();
                    QPoint otherPos = (handleIndex == 0) ? arrow->getLine().p2() : arrow->getLine().p1();
                    const int snapDistance = 10;
                    bool foundSnap = false;
                    QPoint snapTarget;

                    // 检查所有图形的锚点
                    for (size_t i = 0; i < shapes.size(); ++i) {
                        if (i == selectedIndex) continue;
                        if (dynamic_cast<ShapeArrow*>(shapes[i].get())) continue;
                        const auto& arrowAnchors = shapes[i]->getArrowAnchors();
                        for (size_t j = 0; j < arrowAnchors.size(); ++j) {
                            if (arrowAnchors[j].type != ShapeBase::Handle::ArrowAnchor) continue;
                            QPoint target = arrowAnchors[j].rect.center();
                            if (target == otherPos) continue;
                            if ((mousePos - target).manhattanLength() <= snapDistance) {
                                snapTarget = target;
                                snappedHandle = {static_cast<int>(i), static_cast<int>(j), target};
                                foundSnap = true;
                                break;
                            }
                        }
                        if (foundSnap) break;
                    }
                    if (foundSnap) {
                        // 只锁定端点，不再跟随鼠标
                        if (handleIndex == 0) arrow->setP1(snapTarget);
                        else arrow->setP2(snapTarget);
                    } else {
                        // 没有吸附，端点跟随鼠标
                        if (handleIndex == 0) arrow->setP1(mousePos);
                        else arrow->setP2(mousePos);
                        snappedHandle = {-1, -1, QPoint()};
                    }
                    update();
                }
            } else {
                // 处理非箭头图形的缩放
                QPoint delta = event->pos() - lastMousePos;
                shapes[selectedIndex]->handleAnchorInteraction(event->pos(), lastMousePos);
                updateConnectedArrows(selectedIndex, delta);
                lastMousePos = event->pos();
                update();
            }
        } else {
            // 图形整体拖动
            QPoint delta = event->pos() - lastMousePos;
            shapes[selectedIndex]->moveBy(delta);
            updateConnectedArrows(selectedIndex, delta);
            lastMousePos = event->pos();
            update();
        }
    }
}

void DrawingArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (selectedIndex != -1) {
        if (auto* arrow = dynamic_cast<ShapeArrow*>(shapes[selectedIndex].get())) {
            // 如果箭头正在连接到一个图形
            if (snappedHandle.shapeIndex != -1) {
                // 记录连接关系
                ArrowConnection connection;
                connection.arrowIndex = selectedIndex;
                connection.shapeIndex = snappedHandle.shapeIndex;
                connection.handleIndex = snappedHandle.handleIndex;
                connection.isStartPoint = (arrow->getSelectedHandleIndex() == 0);
                arrowConnections.push_back(connection);
            }
        }
        shapes[selectedIndex]->clearHandleSelection();
    }
    snappedHandle = {-1, -1, QPoint()};
    dragging = false;
}

void DrawingArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 查找点击的图形
    for (int i = shapes.size() - 1; i >= 0; --i) {
        if (shapes[i]->contains(event->pos())) {
            // 检查图形是否支持文本编辑
            if (shapes[i]->isTextEditable()) {
                startTextEditing(i);
                return;
            }
        }
    }
}

void DrawingArea::startTextEditing(int shapeIndex)
{
    // 如果已经在编辑其他图形，先完成编辑
    if (m_textEdit) {
        finishTextEditing();
    }

    // 根据图形类型创建不同的文本编辑控件
    if (dynamic_cast<ShapeEllipse*>(shapes[shapeIndex].get())) {
        m_textEdit = new EllipseTextEdit(this);
    } else {
        m_textEdit = new QLineEdit(this);
    }

    // 设置文本编辑控件的基本属性
    m_textEdit->setGeometry(shapes[shapeIndex]->boundingRect().adjusted(5, 5, -5, -5));
    if (auto* lineEdit = qobject_cast<QLineEdit*>(m_textEdit)) {
        lineEdit->setText(shapes[shapeIndex]->getText());
        lineEdit->setAlignment(Qt::AlignCenter);
        lineEdit->show();
        lineEdit->setFocus();

        // 连接编辑完成的信号
        connect(lineEdit, &QLineEdit::editingFinished, this, &DrawingArea::finishTextEditing);
        connect(lineEdit, &QLineEdit::returnPressed, this, &DrawingArea::finishTextEditing);
    }

    // 设置图形为编辑状态
    shapes[shapeIndex]->setEditing(true);
    selectedIndex = shapeIndex;
}

void DrawingArea::finishTextEditing()
{
    if (!m_textEdit || selectedIndex == -1) return;

    // 获取编辑后的文本
    QString newText;
    if (auto* lineEdit = qobject_cast<QLineEdit*>(m_textEdit)) {
        newText = lineEdit->text();
    }
    shapes[selectedIndex]->setText(newText);
    shapes[selectedIndex]->setEditing(false);

    // 清理编辑控件
    m_textEdit->deleteLater();
    m_textEdit = nullptr;

    // 更新显示
    update();
}

void DrawingArea::keyPressEvent(QKeyEvent *event)
{
    if (m_textEdit && m_textEdit->hasFocus()) {
        // 如果正在编辑文本，让文本编辑控件处理键盘事件
        QWidget::keyPressEvent(event);
        return;
    }

    if (selectedIndex != -1 && (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)) {
        // 删除相关的箭头连接
        arrowConnections.erase(
            std::remove_if(arrowConnections.begin(), arrowConnections.end(),
                [this](const ArrowConnection& conn) {
                    return conn.shapeIndex == selectedIndex || conn.arrowIndex == selectedIndex;
                }
            ),
            arrowConnections.end()
        );
        
        shapes.erase(shapes.begin() + selectedIndex);
        selectedIndex = -1;
        update();
    }
}

void DrawingArea::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-shape-type")) {
        event->acceptProposedAction();
    }
}

void DrawingArea::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-shape-type")) {
        QByteArray shapeTypeData = event->mimeData()->data("application/x-shape-type");
        QString shapeType = QString::fromUtf8(shapeTypeData);
        QPoint pos = event->pos();
        std::unique_ptr<ShapeBase> shape;
        QRect defaultRect(pos.x() - 40, pos.y() - 30, 80, 60);
        
        // 暂时只处理矩形和椭圆
        if (shapeType == "rect") {
            shape = ShapeFactory::createRect(defaultRect);
        } else if (shapeType == "ellipse") {
            shape = ShapeFactory::createEllipse(defaultRect);
        } 
        // else if (shapeType == "polygon") {
        //     // 待实现
        // } 
        else if (shapeType == "arrow") {
            QLine line(pos.x() - 40, pos.y(), pos.x() + 40, pos.y());
            shape = ShapeFactory::createArrow(line);
        }

        if (shape) {
            shapes.push_back(std::move(shape));
            selectedIndex = shapes.size() - 1;
            update();
        }
        event->acceptProposedAction();
    }
}

void DrawingArea::updateConnectedArrows(int shapeIndex, const QPoint& delta)
{
    // 遍历所有箭头连接
    for (auto& connection : arrowConnections) {
        if (connection.shapeIndex == shapeIndex) {
            // 找到对应的箭头
            if (auto* arrow = dynamic_cast<ShapeArrow*>(shapes[connection.arrowIndex].get())) {
                // 获取连接的锚点位置
                const auto& anchors = shapes[shapeIndex]->getArrowAnchors();
                if (connection.handleIndex < anchors.size()) {
                    QPoint newAnchorPos = anchors[connection.handleIndex].rect.center();
                    // 更新箭头端点
                    if (connection.isStartPoint) {
                        arrow->setP1(newAnchorPos);
                    } else {
                        arrow->setP2(newAnchorPos);
                    }
                }
            }
        }
    }
} 