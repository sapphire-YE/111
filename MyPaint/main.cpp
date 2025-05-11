#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  // 设置工具按钮样式
  a.setStyleSheet(R"(
QToolButton {
    border: none;
    background: transparent;
    padding: 4px 12px;
}
QToolButton:hover {
    background: #e6f0fa;
}
QToolButton:pressed {
    background: #cce4f7;
}
)");

  MainWindow w;
  w.show();
  return a.exec();
}
