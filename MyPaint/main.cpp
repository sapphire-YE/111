#include "mainwindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  // 设置全局字体，确保中文显示正常
  QFont font = QApplication::font();
  font.setFamily("PingFang SC, Microsoft YaHei, SimHei, sans-serif"); // 使用适合中文的字体
  font.setPointSize(11);                                              // 设置合适的字体大小
  QApplication::setFont(font);

  // 设置工具按钮样式
  a.setStyleSheet(R"(
QToolButton {
    border: none;
    background: transparent;
    padding: 6px 15px;
    color: #333333;
    font-size: 12px;
}
QToolButton:hover {
    background: #e6f0fa;
}
QToolButton:pressed {
    background: #cce4f7;
}
QToolBar {
    border: none;
    spacing: 2px;
    background-color: white;
}
)");

  MainWindow w;
  w.show();
  return a.exec();
}
