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
#include "PropertyPanel.h"

namespace Ui
{
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

    // 缩放相关槽函数
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void onZoomChanged(int zoomIndex);

    // 线条样式相关槽
    void onLineColorButtonClicked();
    void onLineWidthChanged(int width);

private:
    void createMenus();
    void setupToolBar();
    void setupConnections();

    // 辅助函数：根据当前缩放因子更新缩放菜单状态
    void updateZoomMenuState();

    // 标签页居中相关函数
    void centerTabBar();

protected:
    // 重写resizeEvent以在窗口大小改变时保持标签居中
    void resizeEvent(QResizeEvent *event) override;

    Ui::MainWindow *ui;
    DrawingArea *m_drawingArea;
    ShapeLibraryWidget *m_shapeLibrary;
    PropertyPanel *m_propertyPanel;
    QString m_currentFile;

    QAction *actionMoveUp;       // 上移一层
    QAction *actionMoveDown;     // 下移一层
    QAction *actionMoveToTop;    // 移到最顶层
    QAction *actionMoveToBottom; // 移到最底层

    QSpinBox *m_lineWidthSpinBox; // 线条粗细选择器

    // 缩放控制
    QToolButton *m_zoomButton;       // 缩放按钮
    QMenu *m_zoomMenu;               // 缩放菜单
    QAction *m_zoomInAction;         // 放大动作
    QAction *m_zoomOutAction;        // 缩小动作
    QAction *m_zoomResetAction;      // 重置缩放动作
    QAction *m_zoomCustomActions[6]; // 自定义缩放级别动作
};

#endif // MAINWINDOW_H
