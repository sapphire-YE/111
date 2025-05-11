#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DrawingArea.h"
#include "ShapeLibraryWidget.h"
#include "ColorPopupWidget.h"
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_drawingArea(nullptr)
    , m_shapeLibrary(nullptr)
{
    ui->setupUi(this);

    // 创建绘图区
    m_drawingArea = new DrawingArea(nullptr);
    QScrollArea* scrollArea = new QScrollArea(ui->drawingArea);
    scrollArea->setWidget(m_drawingArea);
    scrollArea->setWidgetResizable(true);
    ui->verticalLayoutDrawingArea->addWidget(scrollArea);

    // 创建图形库
    m_shapeLibrary = new ShapeLibraryWidget(this);
    // 替换原有的shapeListWidget
    QLayout* dockLayout = ui->dockWidgetContents->layout();
    if (QWidget* oldWidget = ui->shapeListWidget) {
        dockLayout->removeWidget(oldWidget);
        delete oldWidget;
    }
    dockLayout->addWidget(m_shapeLibrary);

    connect(ui->toolButtonBackgroundColor, &QToolButton::clicked, this, [this](){
        ColorPopupWidget* popup = new ColorPopupWidget(this);
        popup->setWindowFlags(Qt::Popup);
        QPoint pos = ui->toolButtonBackgroundColor->mapToGlobal(QPoint(0, ui->toolButtonBackgroundColor->height()));
        popup->move(pos);
        connect(popup, &ColorPopupWidget::colorSelected, this, [this, popup](const QColor& color){
            m_drawingArea->setBackgroundColor(color);
            popup->close();
            popup->deleteLater();
        });
        popup->show();
    });

    // 连接网格大小按钮
    connect(ui->actionGridSmall, &QAction::triggered, this, [this](){
        m_drawingArea->setGridSize(10);
    });
    connect(ui->actionGridMedium, &QAction::triggered, this, [this](){
        m_drawingArea->setGridSize(20);
    });
    connect(ui->actionGridLarge, &QAction::triggered, this, [this](){
        m_drawingArea->setGridSize(40);
    });

    // 连接页面大小按钮
    connect(ui->actionA3, &QAction::triggered, this, [this](){
        m_drawingArea->setPageSize(QSize(1500, 2100));
    });
    connect(ui->actionA4, &QAction::triggered, this, [this](){
        m_drawingArea->setPageSize(QSize(1050, 1500));
    });
    connect(ui->actionA5, &QAction::triggered, this, [this](){
        m_drawingArea->setPageSize(QSize(750, 1050));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

