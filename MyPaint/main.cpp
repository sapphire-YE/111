#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序的语言环境为系统默认语言
    QLocale locale = QLocale::system();
    QLocale::setDefault(locale);

    // 加载 Qt 内置翻译
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + locale.name(),
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        app.installTranslator(&qtTranslator);
    }

    // 加载应用程序翻译
    QTranslator translator;
    QString qmPath = ":/translations/mypaint_" + locale.name();
    qDebug() << "Loading translation from:" << qmPath;

    if (translator.load(qmPath))
    {
        qDebug() << "Translation loaded successfully";
        app.installTranslator(&translator);
    }
    else
    {
        qDebug() << "Failed to load translation";
    }

    MainWindow w;
    w.show();
    return app.exec();
}
