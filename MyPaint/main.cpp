#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QDebug>
#include <QFile>

QString getQmlPath()
{
    //这里为调试目录
    QString appDir = QCoreApplication::applicationDirPath();
    QDir currentDir(appDir);

    QString qmlFilePath = QDir::cleanPath(currentDir.absolutePath() + QDir::separator() +
        QString("mui") + QDir::separator() +
        QString("zh_CN") + QDir::separator() +
        QString("lessoncode.qm")
    );

    if (QFile::exists(qmlFilePath))
    {
        return qmlFilePath;
    }

    return QString();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;

    if (translator.load(getQmlPath()))
    {
        a.installTranslator(&translator);
    }

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
