#include "mainwindow.h"
#include "ColorPopupWidget.h"
#include "DrawingArea.h"
#include "ShapeLibraryWidget.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QToolBar>
#include <QSpinBox>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_drawingArea(nullptr), m_shapeLibrary(nullptr), m_currentFile("")
{
  ui->setupUi(this);
  createMenus();
  setupToolBar();
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
    ColorPopupWidget *popup = new ColorPopupWidget(this);
    popup->setWindowFlags(Qt::Popup);
    QPoint pos = ui->toolButtonBackgroundColor->mapToGlobal(
        QPoint(0, ui->toolButtonBackgroundColor->height()));
    popup->move(pos);
    connect(popup, &ColorPopupWidget::colorSelected, this,
            [this, popup](const QColor &color) {
              m_drawingArea->setBackgroundColor(color);
              popup->close();
              popup->deleteLater();
            });
    popup->show(); });

  // 连接网格大小按钮
  connect(ui->actionGridSmall, &QAction::triggered, this,
          [this]()
          { m_drawingArea->setGridSize(10); });
  connect(ui->actionGridMedium, &QAction::triggered, this,
          [this]()
          { m_drawingArea->setGridSize(20); });
  connect(ui->actionGridLarge, &QAction::triggered, this,
          [this]()
          { m_drawingArea->setGridSize(40); });

  // 连接网格显示/隐藏按钮
  connect(ui->toolButtonGridVisible, &QToolButton::toggled, this,
          [this](bool checked)
          {
            m_drawingArea->setGridVisible(checked);
            ui->toolButtonGridVisible->setText(checked ? "Hide Grid"
                                                       : "Show Grid");
          });

  // 连接页面大小按钮
  connect(ui->actionA3, &QAction::triggered, this,
          [this]()
          { m_drawingArea->setPageSize(QSize(1500, 2100)); });
  connect(ui->actionA4, &QAction::triggered, this,
          [this]()
          { m_drawingArea->setPageSize(QSize(1050, 1500)); });
  connect(ui->actionA5, &QAction::triggered, this,
          [this]()
          { m_drawingArea->setPageSize(QSize(750, 1050)); });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::createMenus()
{
  // 创建文件菜单
  QMenu *fileMenu = menuBar()->addMenu("文件");
  fileMenu->addAction("新建", this, &MainWindow::onNewFile, QKeySequence::New);
  fileMenu->addAction("打开", this, &MainWindow::onOpenFile,
                      QKeySequence::Open);
  fileMenu->addAction("保存", this, &MainWindow::onSaveFile,
                      QKeySequence::Save);
  fileMenu->addAction("另存为", this, &MainWindow::onSaveAs,
                      QKeySequence::SaveAs);
  fileMenu->addSeparator();
  fileMenu->addAction("导出为PNG", this, &MainWindow::onExportPNG);
  fileMenu->addAction("导出为SVG", this, &MainWindow::onExportSVG);

  // 创建排列菜单
  QMenu *arrangeMenu = menuBar()->addMenu("排列");
  actionMoveUp = arrangeMenu->addAction("上移一层", this, &MainWindow::onMoveUp,
                                        QKeySequence(Qt::CTRL + Qt::Key_Up));
  actionMoveDown =
      arrangeMenu->addAction("下移一层", this, &MainWindow::onMoveDown,
                             QKeySequence(Qt::CTRL + Qt::Key_Down));
  actionMoveToTop =
      arrangeMenu->addAction("移到最顶层", this, &MainWindow::onMoveToTop,
                             QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Up));
  actionMoveToBottom =
      arrangeMenu->addAction("移到最底层", this, &MainWindow::onMoveToBottom,
                             QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Down));

  // 创建开始菜单（应用内部）
  QMenu *startMenu = new QMenu(this);

  // 直接添加排列选项到开始菜单
  startMenu->addAction("上移一层", this, &MainWindow::onMoveUp,
                       QKeySequence(Qt::CTRL + Qt::Key_Up));
  startMenu->addAction("下移一层", this, &MainWindow::onMoveDown,
                       QKeySequence(Qt::CTRL + Qt::Key_Down));
  startMenu->addAction("移到最顶层", this, &MainWindow::onMoveToTop,
                       QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Up));
  startMenu->addAction("移到最底层", this, &MainWindow::onMoveToBottom,
                       QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Down));

  // 创建排列按钮
  QToolButton *arrangeButton = new QToolButton(this);
  arrangeButton->setText("排列");
  arrangeButton->setPopupMode(QToolButton::InstantPopup);
  arrangeButton->setMenu(startMenu);
  ui->startToolBar->addWidget(arrangeButton);
}

void MainWindow::setupToolBar()
{
  // 创建线条样式菜单
  QMenu *lineStyleMenu = new QMenu(this);

  // 添加线条颜色选项
  QAction *lineColorAction = lineStyleMenu->addAction("线条颜色");
  connect(lineColorAction, &QAction::triggered, this, &MainWindow::onLineColorButtonClicked);

  // 添加分隔符
  lineStyleMenu->addSeparator();

  // 添加线条粗细选项
  QWidget *lineWidthWidget = new QWidget(this);
  QHBoxLayout *lineWidthLayout = new QHBoxLayout(lineWidthWidget);
  lineWidthLayout->setContentsMargins(12, 3, 12, 3);

  QLabel *lineWidthLabel = new QLabel("线条宽度:", lineWidthWidget);
  lineWidthLayout->addWidget(lineWidthLabel);

  m_lineWidthSpinBox = new QSpinBox(lineWidthWidget);
  m_lineWidthSpinBox->setRange(1, 10);
  m_lineWidthSpinBox->setValue(1);
  m_lineWidthSpinBox->setFixedWidth(50);
  lineWidthLayout->addWidget(m_lineWidthSpinBox);

  QWidgetAction *widthAction = new QWidgetAction(this);
  widthAction->setDefaultWidget(lineWidthWidget);
  lineStyleMenu->addAction(widthAction);

  // 创建线条样式按钮
  QToolButton *lineStyleButton = new QToolButton(this);
  lineStyleButton->setText("线条样式");
  lineStyleButton->setToolTip("设置选中图形的线条样式");
  lineStyleButton->setPopupMode(QToolButton::InstantPopup);
  lineStyleButton->setMenu(lineStyleMenu);
  ui->startToolBar->addWidget(lineStyleButton);

  // 连接信号和槽
  connect(m_lineWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onLineWidthChanged);
}

void MainWindow::setupConnections()
{
  // 连接文件操作按钮
  connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onNewFile);
  connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
  connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveFile);

  // 连接导出按钮
  connect(ui->actionExport, &QAction::triggered, this, [this]()
          {
    QMenu *exportMenu = new QMenu(this);
    exportMenu->addAction("导出为PNG", this, &MainWindow::onExportPNG);
    exportMenu->addAction("导出为SVG", this, &MainWindow::onExportSVG);

    // 获取工具栏按钮的位置
    QToolBar *toolbar = ui->startToolBar;
    QWidget *button = toolbar->widgetForAction(ui->actionExport);
    if (button) {
      QPoint pos = button->mapToGlobal(QPoint(0, button->height()));
      exportMenu->exec(pos);
    } });
}

void MainWindow::onNewFile()
{
  if (m_drawingArea)
  {
    m_drawingArea->clear();
    m_currentFile.clear();
    setWindowTitle("未命名 - 流程图");
  }
}

void MainWindow::onOpenFile()
{
  QString fileName = QFileDialog::getOpenFileName(
      this, "打开流程图", "", "流程图文件 (*.flow);;所有文件 (*)");

  if (!fileName.isEmpty())
  {
    if (m_drawingArea && m_drawingArea->loadFromFile(fileName))
    {
      m_currentFile = fileName;
      setWindowTitle(QFileInfo(fileName).fileName() + " - 流程图");
    }
    else
    {
      QMessageBox::warning(this, "错误", "无法打开文件");
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
      QMessageBox::warning(this, "错误", "保存文件失败");
    }
  }
}

void MainWindow::onSaveAs()
{
  QString fileName = QFileDialog::getSaveFileName(
      this, "保存流程图", "", "流程图文件 (*.flow);;所有文件 (*)");

  if (!fileName.isEmpty())
  {
    if (m_drawingArea && m_drawingArea->saveToFile(fileName))
    {
      m_currentFile = fileName;
      setWindowTitle(QFileInfo(fileName).fileName() + " - 流程图");
    }
    else
    {
      QMessageBox::warning(this, "错误", "保存文件失败");
    }
  }
}

void MainWindow::onExportPNG()
{
  QString fileName =
      QFileDialog::getSaveFileName(this, "导出PNG", "", "PNG图片 (*.png)");

  if (!fileName.isEmpty() && m_drawingArea)
  {
    if (!m_drawingArea->exportToPNG(fileName))
    {
      QMessageBox::warning(this, "错误", "导出PNG失败");
    }
  }
}

void MainWindow::onExportSVG()
{
  QString fileName =
      QFileDialog::getSaveFileName(this, "导出SVG", "", "SVG文件 (*.svg)");

  if (!fileName.isEmpty() && m_drawingArea)
  {
    if (!m_drawingArea->exportToSVG(fileName))
    {
      QMessageBox::warning(this, "错误", "导出SVG失败");
    }
  }
}

// 实现排列相关的槽函数
void MainWindow::onMoveUp() { m_drawingArea->moveShapeUp(); }

void MainWindow::onMoveDown() { m_drawingArea->moveShapeDown(); }

void MainWindow::onMoveToTop() { m_drawingArea->moveShapeToTop(); }

void MainWindow::onMoveToBottom()
{
  m_drawingArea->moveShapeToBottom();
}

// 实现线条样式相关的槽函数
void MainWindow::onLineColorButtonClicked()
{
  ColorPopupWidget *popup = new ColorPopupWidget(this);
  popup->setWindowFlags(Qt::Popup);
  QPoint pos = QCursor::pos();
  popup->move(pos);
  connect(popup, &ColorPopupWidget::colorSelected, this, [this, popup](const QColor &color)
          {
        m_drawingArea->setSelectedShapeLineColor(color);
        popup->close();
        popup->deleteLater(); });
  popup->show();
}

void MainWindow::onLineWidthChanged(int width)
{
  m_drawingArea->setSelectedShapeLineWidth(width);
}
