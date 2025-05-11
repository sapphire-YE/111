#include "ShapeBase.h"

void ShapeBase::paint(QPainter* painter, bool selected)
{
    if (!painter) return;

    // 1. 先绘制图形本身
    paintShape(painter);

    // 2. 如果图形有文本，绘制文本
    if (!m_text.isEmpty()) {
        painter->setPen(QPen(Qt::black));  // 设置黑色文本
        painter->setBrush(Qt::NoBrush);    // 文本不需要填充
        QRect textRect = boundingRect().adjusted(5, 5, -5, -5);  // 留出边距
        painter->drawText(textRect, Qt::AlignCenter, m_text);
    }

    // 3. 如果被选中，绘制选中状态
    if (selected) {
        // 绘制虚线框
        painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());

        // 绘制所有锚点
        for (const auto& handle : getHandles()) {
            if (handle.type == Handle::Scale) {
                // 绘制缩放锚点（白色填充的蓝色边框方块）
                painter->setBrush(Qt::white);
                painter->setPen(QPen(Qt::blue));
                painter->drawRect(handle.rect);
            } else if (handle.type == Handle::Arrow) {
                // 绘制箭头锚点（灰色加号）
                painter->setPen(QPen(Qt::gray, 2));
                painter->setBrush(Qt::NoBrush);
                QPoint center = handle.rect.center();
                painter->drawLine(center.x() - 5, center.y(), center.x() + 5, center.y());
                painter->drawLine(center.x(), center.y() - 5, center.x(), center.y() + 5);
            }
        }
    }
}

bool ShapeBase::handleAnchorInteraction(const QPoint& mousePos, const QPoint& lastMousePos)
{
    if (m_selectedHandleIndex == -1) return false;

    // 计算新的矩形区域
    QRect newRect = calculateNewRect(mousePos, lastMousePos);
    if (newRect.isEmpty()) return false;

    // 调用子类的resize方法
    resize(newRect);
    return true;
}

QRect ShapeBase::calculateNewRect(const QPoint& mousePos, const QPoint& lastMousePos) const
{
    QRect currentRect = boundingRect();
    QPoint delta = mousePos - lastMousePos;
    QRect newRect = currentRect;

    // 根据选中的锚点类型和位置计算新的矩形
    switch (m_selectedHandleIndex) {
        case 0: // 左上
            newRect.setTopLeft(newRect.topLeft() + delta);
            break;
        case 1: // 上中
            newRect.setTop(newRect.top() + delta.y());
            break;
        case 2: // 右上
            newRect.setTopRight(newRect.topRight() + delta);
            break;
        case 3: // 左中
            newRect.setLeft(newRect.left() + delta.x());
            break;
        case 4: // 右中
            newRect.setRight(newRect.right() + delta.x());
            break;
        case 5: // 左下
            newRect.setBottomLeft(newRect.bottomLeft() + delta);
            break;
        case 6: // 下中
            newRect.setBottom(newRect.bottom() + delta.y());
            break;
        case 7: // 右下
            newRect.setBottomRight(newRect.bottomRight() + delta);
            break;
        default:
            return QRect();
    }

    // 确保矩形不会翻转
    if (newRect.width() < 1 || newRect.height() < 1) {
        return currentRect;
    }

    return newRect;
}

std::vector<ShapeBase::Handle> ShapeBase::getHandles() const {
    std::vector<Handle> handles;
    QRect rect = boundingRect();
    int w = rect.width(), h = rect.height();
    int x = rect.left(), y = rect.top();
    int size = 8;  // 锚点大小

    // 八个缩放锚点
    handles.push_back({QRect(x - 4, y - 4, size, size), Handle::Scale, 0});  // 左上
    handles.push_back({QRect(x + w/2 - 4, y - 4, size, size), Handle::Scale, 1});  // 上中
    handles.push_back({QRect(x + w - 4, y - 4, size, size), Handle::Scale, 2});  // 右上
    handles.push_back({QRect(x - 4, y + h/2 - 4, size, size), Handle::Scale, 3});  // 左中
    handles.push_back({QRect(x + w - 4, y + h/2 - 4, size, size), Handle::Scale, 4});  // 右中
    handles.push_back({QRect(x - 4, y + h - 4, size, size), Handle::Scale, 5});  // 左下
    handles.push_back({QRect(x + w/2 - 4, y + h - 4, size, size), Handle::Scale, 6});  // 下中
    handles.push_back({QRect(x + w - 4, y + h - 4, size, size), Handle::Scale, 7});  // 右下

    // 只在需要时添加加号锚点
    if (needPlusHandles()) {
        int arrowSize = 24; // 加号锚点区域大小
        int offset = 23;    // 距离边界的距离（加大）
        // 上
        handles.push_back({QRect(x + w/2 - arrowSize/2, y - offset, arrowSize, arrowSize), Handle::Arrow, 8});
        // 下
        handles.push_back({QRect(x + w/2 - arrowSize/2, y + h + offset - arrowSize, arrowSize, arrowSize), Handle::Arrow, 9});
        // 左
        handles.push_back({QRect(x - offset, y + h/2 - arrowSize/2, arrowSize, arrowSize), Handle::Arrow, 10});
        // 右
        handles.push_back({QRect(x + w + offset - arrowSize, y + h/2 - arrowSize/2, arrowSize, arrowSize), Handle::Arrow, 11});
    }
    return handles;
} 