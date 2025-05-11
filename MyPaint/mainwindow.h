#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// 。h
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class DrawingArea;
class ShapeLibraryWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveAs();
    void onExportPNG();
    void onExportSVG();

private:
    void createMenus();
    void setupConnections();

    Ui::MainWindow *ui;
    DrawingArea *m_drawingArea;
    ShapeLibraryWidget *m_shapeLibrary;
    QString m_currentFile; // 当前打开的文件路径
};

#endif // MAINWINDOW_H
