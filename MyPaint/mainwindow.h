#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//。h
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DrawingArea;
class ShapeLibraryWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DrawingArea* m_drawingArea;
    ShapeLibraryWidget* m_shapeLibrary;
};

#endif // MAINWINDOW_H
