#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DrawingArea.h"
#include "ShapeLibraryWidget.h"
#include "ColorPopupWidget.h"
#include <QScrollArea>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_drawingArea(nullptr), m_shapeLibrary(nullptr), m_currentFile("")
{
    ui->setupUi(this);
    createMenus();
    setupConnections();

    // 创建绘图区
    m_drawingArea = new DrawingArea(nullptr);
    QScrollArea *scrollArea = new QScrollArea(ui->drawingArea);
    scrollArea->setWidget(m_drawingArea);
    scrollArea->setWidgetResizable(true);
    ui->verticalLayoutDrawingArea->addWidget(scrollArea);

    // 创建图形库
    m_shapeLibrary = new ShapeLibraryWidget(this);
    // 替换原有的shapeListWidget
    QLayout *dockLayout = ui->dockWidgetContents->layout();
    if (QWidget *oldWidget = ui->shapeListWidget)
    {
        dockLayout->removeWidget(oldWidget);
        delete oldWidget;
    }
    dockLayout->addWidget(m_shapeLibrary);

    connect(ui->toolButtonBackgroundColor, &QToolButton::clicked, this, [this]()
            {
        ColorPopupWidget* popup = new ColorPopupWidget(this);
        popup->setWindowFlags(Qt::Popup);
        QPoint pos = ui->toolButtonBackgroundColor->mapToGlobal(QPoint(0, ui->toolButtonBackgroundColor->height()));
        popup->move(pos);
        connect(popup, &ColorPopupWidget::colorSelected, this, [this, popup](const QColor& color){
            m_drawingArea->setBackgroundColor(color);
            popup->close();
            popup->deleteLater();
        });
        popup->show(); });

    // 连接网格大小按钮
    connect(ui->actionGridSmall, &QAction::triggered, this, [this]()
            { m_drawingArea->setGridSize(10); });
    connect(ui->actionGridMedium, &QAction::triggered, this, [this]()
            { m_drawingArea->setGridSize(20); });
    connect(ui->actionGridLarge, &QAction::triggered, this, [this]()
            { m_drawingArea->setGridSize(40); });

    // 连接页面大小按钮
    connect(ui->actionA3, &QAction::triggered, this, [this]()
            { m_drawingArea->setPageSize(QSize(1500, 2100)); });
    connect(ui->actionA4, &QAction::triggered, this, [this]()
            { m_drawingArea->setPageSize(QSize(1050, 1500)); });
    connect(ui->actionA5, &QAction::triggered, this, [this]()
            { m_drawingArea->setPageSize(QSize(750, 1050)); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenus()
{
    // 创建文件菜单
    QMenu *fileMenu = menuBar()->addMenu(tr("文件(&F)"));

    QAction *saveAsAction = fileMenu->addAction(tr("另存为(&A)"), this, &MainWindow::onSaveAs);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
}

void MainWindow::setupConnections()
{
    // 连接文件操作按钮
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onNewFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveFile);

    // 连接导出按钮
    connect(ui->actionExport, &QAction::triggered, this, [this]() {
        QMenu* exportMenu = new QMenu(this);
        exportMenu->addAction(tr("导出为PNG"), this, &MainWindow::onExportPNG);
        exportMenu->addAction(tr("导出为SVG"), this, &MainWindow::onExportSVG);
        
        // 获取工具栏按钮的位置
        QToolBar* toolbar = ui->startToolBar;
        QWidget* button = toolbar->widgetForAction(ui->actionExport);
        if (button) {
            QPoint pos = button->mapToGlobal(QPoint(0, button->height()));
            exportMenu->exec(pos);
        }
    });
}

void MainWindow::onNewFile()
{
    if (m_drawingArea)
    {
        m_drawingArea->clear();
        m_currentFile.clear();
        setWindowTitle(tr("未命名 - 流程图"));
    }
}

void MainWindow::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("打开流程图"), "",
                                                    tr("流程图文件 (*.flow);;所有文件 (*)"));

    if (!fileName.isEmpty())
    {
        if (m_drawingArea && m_drawingArea->loadFromFile(fileName))
        {
            m_currentFile = fileName;
            setWindowTitle(QFileInfo(fileName).fileName() + tr(" - 流程图"));
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("无法打开文件"));
        }
    }
}

void MainWindow::onSaveFile()
{
    if (m_currentFile.isEmpty())
    {
        onSaveAs();
    }
    else
    {
        if (m_drawingArea && !m_drawingArea->saveToFile(m_currentFile))
        {
            QMessageBox::warning(this, tr("错误"), tr("保存文件失败"));
        }
    }
}

void MainWindow::onSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存流程图"), "",
                                                    tr("流程图文件 (*.flow);;所有文件 (*)"));

    if (!fileName.isEmpty())
    {
        if (m_drawingArea && m_drawingArea->saveToFile(fileName))
        {
            m_currentFile = fileName;
            setWindowTitle(QFileInfo(fileName).fileName() + tr(" - 流程图"));
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("保存文件失败"));
        }
    }
}

void MainWindow::onExportPNG()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("导出PNG"), "",
                                                    tr("PNG图片 (*.png)"));

    if (!fileName.isEmpty() && m_drawingArea)
    {
        if (!m_drawingArea->exportToPNG(fileName))
        {
            QMessageBox::warning(this, tr("错误"), tr("导出PNG失败"));
        }
    }
}

void MainWindow::onExportSVG()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("导出SVG"), "",
                                                    tr("SVG文件 (*.svg)"));

    if (!fileName.isEmpty() && m_drawingArea)
    {
        if (!m_drawingArea->exportToSVG(fileName))
        {
            QMessageBox::warning(this, tr("错误"), tr("导出SVG失败"));
        }
    }
}
