#include "mainwindow.h"
#include "ColorPopupWidget.h"
#include "DrawingArea.h"
#include "ShapeLibraryWidget.h"
#include "PropertyPanel.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QToolBar>
#include <QSpinBox>
#include <QLabel>
#include <QShortcut>
#include <QActionGroup>
#include <QShortcut>
#include <QFile>
#include <QTabBar>
#include <QProxyStyle>
#include <QStyleOptionTab>
#include <QResizeEvent>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_drawingArea(nullptr), m_shapeLibrary(nullptr),
      m_propertyPanel(nullptr), m_currentFile("")
{
  ui->setupUi(this);

  // 应用标签页样式
  QFile styleFile(":/styles/tabstyle.css");
  if (styleFile.open(QFile::ReadOnly))
  {
    QString styleSheet = QString::fromUtf8(styleFile.readAll());
    ui->ribbonTabWidget->setStyleSheet(styleSheet);
    styleFile.close();
  }

  // 设置标签页居中显示
  ui->ribbonTabWidget->tabBar()->setExpanding(false); // 禁止标签拉伸填充
  ui->ribbonTabWidget->tabBar()->setDocumentMode(true);

  // 手动设置标签居中显示
  QTabBar *tabBar = ui->ribbonTabWidget->tabBar();
  tabBar->setDrawBase(false);

  // 使Tab居中的方法：将标签栏水平对齐方式设为居中
  tabBar->setUsesScrollButtons(false);
  tabBar->setStyleSheet(tabBar->styleSheet() + " QTabBar::tab { min-width: 120px; }");

  // 在窗口显示后，调用centerTabBar调整标签位置
  QTimer::singleShot(100, this, &MainWindow::centerTabBar);

  createMenus();
  setupToolBar();
  setupConnections();

  // 创建绘图区
  m_drawingArea = new DrawingArea(nullptr);
  QScrollArea *scrollArea = new QScrollArea(ui->drawingArea);
  scrollArea->setWidget(m_drawingArea);
  scrollArea->setWidgetResizable(true);
  ui->verticalLayoutDrawingArea->addWidget(scrollArea);

  // 连接缩放因子变化信号
  connect(m_drawingArea, &DrawingArea::zoomFactorChanged, this, [this](double)
          { updateZoomMenuState(); });

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

  // 创建属性面板
  m_propertyPanel = new PropertyPanel(this);
  m_propertyPanel->setDrawingArea(m_drawingArea);
  // 替换原有的属性标签
  QLayout *propertyDockLayout = ui->dockWidgetContents_2->layout();
  if (QWidget *oldPropertyWidget = ui->propertyLabel)
  {
    propertyDockLayout->removeWidget(oldPropertyWidget);
    delete oldPropertyWidget;
  }
  propertyDockLayout->addWidget(m_propertyPanel);

  // 连接图形选择信号
  connect(m_drawingArea, &DrawingArea::shapeSelected, m_propertyPanel, &PropertyPanel::updateForSelectedShape);
  connect(m_drawingArea, &DrawingArea::selectionCleared, m_propertyPanel, &PropertyPanel::clearProperties);

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

  // 添加缩放快捷键
  QShortcut *zoomInShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);
  connect(zoomInShortcut, &QShortcut::activated, this, &MainWindow::onZoomIn);

  QShortcut *zoomOutShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), this);
  connect(zoomOutShortcut, &QShortcut::activated, this, &MainWindow::onZoomOut);

  QShortcut *zoomResetShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_0), this);
  connect(zoomResetShortcut, &QShortcut::activated, this, &MainWindow::onZoomReset);

  // 连接DrawingArea属性变更信号到PropertyPanel
  connect(m_drawingArea, &DrawingArea::backgroundColorChanged, this, [this](const QColor &color)
          {
    if (m_propertyPanel) {
      // 更新PropertyPanel的背景色设置
      m_propertyPanel->updateBackgroundColorUI(color);
    } });

  connect(m_drawingArea, &DrawingArea::gridVisibilityChanged, this, [this](bool visible)
          {
    if (m_propertyPanel) {
      // 更新PropertyPanel的网格可见性设置
      m_propertyPanel->updateGridVisibilityUI(visible);
    }
    // 同步更新工具栏的网格可见性按钮
    ui->toolButtonGridVisible->setChecked(visible);
    ui->toolButtonGridVisible->setText(visible ? "Hide Grid" : "Show Grid"); });

  connect(m_drawingArea, &DrawingArea::gridSizeChanged, this, [this](int size)
          {
    if (m_propertyPanel) {
      // 更新PropertyPanel的网格大小设置
      m_propertyPanel->updateGridSizeUI(size);
    } });

  connect(m_drawingArea, &DrawingArea::pageSizeChanged, this, [this](const QSize &size)
          {
    if (m_propertyPanel) {
      // 更新PropertyPanel的页面大小设置
      m_propertyPanel->updatePageSizeUI(size);
    } });
}

MainWindow::~MainWindow()
{
  delete ui;
  // DrawingArea 和 PropertyPanel 在UI中，会自动释放
}

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

  // 向arrangeToolBar添加排列功能按钮
  ui->arrangeToolBar->addSeparator();

  // 添加"上移"按钮
  QToolButton *moveUpButton = new QToolButton(this);
  moveUpButton->setText("上移");
  moveUpButton->setToolTip("上移一层");
  connect(moveUpButton, &QToolButton::clicked, this, &MainWindow::onMoveUp);
  ui->arrangeToolBar->addWidget(moveUpButton);

  // 添加"下移"按钮
  QToolButton *moveDownButton = new QToolButton(this);
  moveDownButton->setText("下移");
  moveDownButton->setToolTip("下移一层");
  connect(moveDownButton, &QToolButton::clicked, this, &MainWindow::onMoveDown);
  ui->arrangeToolBar->addWidget(moveDownButton);

  // 添加"置顶"按钮
  QToolButton *moveToTopButton = new QToolButton(this);
  moveToTopButton->setText("置顶");
  moveToTopButton->setToolTip("置于顶层");
  connect(moveToTopButton, &QToolButton::clicked, this, &MainWindow::onMoveToTop);
  ui->arrangeToolBar->addWidget(moveToTopButton);

  // 添加"置底"按钮
  QToolButton *moveToBottomButton = new QToolButton(this);
  moveToBottomButton->setText("置底");
  moveToBottomButton->setToolTip("置于底层");
  connect(moveToBottomButton, &QToolButton::clicked, this, &MainWindow::onMoveToBottom);
  ui->arrangeToolBar->addWidget(moveToBottomButton);

  // 连接信号和槽
  connect(m_lineWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onLineWidthChanged);

  // 创建缩放控制菜单和按钮 - 添加到Page选项卡
  m_zoomMenu = new QMenu(this);

  // 添加基本缩放操作
  m_zoomInAction = m_zoomMenu->addAction("放大", this, &MainWindow::onZoomIn, QKeySequence(Qt::CTRL + Qt::Key_Plus));
  m_zoomOutAction = m_zoomMenu->addAction("缩小", this, &MainWindow::onZoomOut, QKeySequence(Qt::CTRL + Qt::Key_Minus));
  m_zoomResetAction = m_zoomMenu->addAction("重置缩放 (100%)", this, &MainWindow::onZoomReset, QKeySequence(Qt::CTRL + Qt::Key_0));

  m_zoomMenu->addSeparator();

  // 添加预设缩放级别
  QActionGroup *zoomGroup = new QActionGroup(this);
  const double zoomLevels[] = {0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
  const QString zoomTexts[] = {"50%", "75%", "100%", "125%", "150%", "200%"};

  for (int i = 0; i < 6; i++)
  {
    m_zoomCustomActions[i] = m_zoomMenu->addAction(zoomTexts[i]);
    m_zoomCustomActions[i]->setCheckable(true);
    zoomGroup->addAction(m_zoomCustomActions[i]);
    connect(m_zoomCustomActions[i], &QAction::triggered, this, [this, i]()
            { onZoomChanged(i); });
  }

  // 初始选择100%
  m_zoomCustomActions[2]->setChecked(true);

  // 创建缩放按钮并添加到Page选项卡
  m_zoomButton = new QToolButton(ui->tabPage);
  m_zoomButton->setText("缩放");
  m_zoomButton->setToolTip("设置页面缩放比例");
  m_zoomButton->setPopupMode(QToolButton::InstantPopup);
  m_zoomButton->setMenu(m_zoomMenu);
  ui->horizontalLayoutPage->addWidget(m_zoomButton);
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

// 放大
void MainWindow::onZoomIn()
{
  if (m_drawingArea)
  {
    m_drawingArea->zoomIn();
    updateZoomMenuState();
  }
}

// 缩小
void MainWindow::onZoomOut()
{
  if (m_drawingArea)
  {
    m_drawingArea->zoomOut();
    updateZoomMenuState();
  }
}

// 重置缩放
void MainWindow::onZoomReset()
{
  if (m_drawingArea)
  {
    m_drawingArea->resetZoom();
    updateZoomMenuState();
  }
}

// 根据选择的缩放比例改变缩放
void MainWindow::onZoomChanged(int zoomIndex)
{
  if (!m_drawingArea)
    return;

  // 预设缩放比例值：50%, 75%, 100%, 125%, 150%, 200%
  double zoomFactors[] = {0.5, 0.75, 1.0, 1.25, 1.5, 2.0};

  if (zoomIndex >= 0 && zoomIndex < sizeof(zoomFactors) / sizeof(double))
  {
    m_drawingArea->setZoomFactor(zoomFactors[zoomIndex]);

    // 更新菜单中的勾选状态
    for (int i = 0; i < 6; i++)
    {
      m_zoomCustomActions[i]->setChecked(i == zoomIndex);
    }
  }
}

// 辅助函数：根据当前缩放因子更新缩放菜单状态
void MainWindow::updateZoomMenuState()
{
  if (!m_drawingArea)
    return;

  double currentZoom = m_drawingArea->getZoomFactor();

  // 预设缩放值
  const double zoomLevels[] = {0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
  bool found = false;

  // 查找当前缩放因子是否与某个预设值匹配
  for (int i = 0; i < 6; i++)
  {
    // 使用一个小的浮点数误差允许范围
    if (std::abs(currentZoom - zoomLevels[i]) < 0.001)
    {
      m_zoomCustomActions[i]->setChecked(true);
      found = true;
    }
    else
    {
      m_zoomCustomActions[i]->setChecked(false);
    }
  }

  // 如果没有匹配的预设值，取消所有勾选
  if (!found)
  {
    for (int i = 0; i < 6; i++)
    {
      m_zoomCustomActions[i]->setChecked(false);
    }
  }
}

// 实现标签页居中功能
void MainWindow::centerTabBar()
{
  QTabBar *tabBar = ui->ribbonTabWidget->tabBar();
  int totalWidth = 0;

  // 计算所有标签的总宽度
  for (int i = 0; i < tabBar->count(); i++)
  {
    totalWidth += tabBar->tabRect(i).width();
  }

  // 计算左侧应该留出的空间，使标签整体居中
  int leftMargin = (width() - totalWidth) / 2;
  if (leftMargin < 0)
    leftMargin = 0;

  // 设置标签栏的样式表，添加左侧边距
  QString style = QString("QTabBar { margin-left: %1px; }").arg(leftMargin);
  tabBar->setStyleSheet(tabBar->styleSheet() + style);
}

// 当窗口大小改变时，重新调整标签页位置
void MainWindow::resizeEvent(QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);
  centerTabBar(); // 重新计算并应用标签居中
}
