#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidgetAction>
#include "DrawingArea.h"
#include "ShapeLibraryWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveAs();
    void onExportPNG();
    void onExportSVG();
    void onMoveUp();
    void onMoveDown();
    void onMoveToTop();
    void onMoveToBottom();
    
    // 线条样式相关槽
    void onLineColorButtonClicked();
    void onLineWidthChanged(int width);

private:
    void createMenus();
    void setupToolBar();
    void setupConnections();

    Ui::MainWindow *ui;
    DrawingArea *m_drawingArea;
    ShapeLibraryWidget *m_shapeLibrary;
    QString m_currentFile;

    QAction* actionMoveUp;          // 上移一层
    QAction* actionMoveDown;        // 下移一层
    QAction* actionMoveToTop;       // 移到最顶层
    QAction* actionMoveToBottom;    // 移到最底层
    
    QSpinBox* m_lineWidthSpinBox;   // 线条粗细选择器
};

#endif // MAINWINDOW_H
