#ifndef ELLIPSETEXTEDIT_H
#define ELLIPSETEXTEDIT_H

#include <QLineEdit>
#include <QPainter>
#include <QRegion>

class EllipseTextEdit : public QLineEdit
{
  Q_OBJECT
public:
  explicit EllipseTextEdit(QWidget *parent = nullptr) : QLineEdit(parent)
  {
    setFrame(false);                           // 移除默认边框
    setStyleSheet("background: transparent;"); // 设置背景透明
  }

protected:
  void paintEvent(QPaintEvent *event) override
  {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制白色椭圆背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(rect());

    // 绘制文本
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    QRect textRect = rect().adjusted(5, 5, -5, -5);
    painter.drawText(textRect, Qt::AlignCenter, text());
  }

  void resizeEvent(QResizeEvent *event) override
  {
    QLineEdit::resizeEvent(event);
    // 设置椭圆形状的遮罩
    QRegion region(0, 0, width(), height(), QRegion::Ellipse);
    setMask(region);
  }
};

#endif // ELLIPSETEXTEDIT_H