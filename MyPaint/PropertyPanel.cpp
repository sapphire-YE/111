#include "PropertyPanel.h"
#include "DrawingArea.h"
#include "ShapeRect.h"
#include "ShapeEllipse.h"
#include "ShapeArrow.h"
#include "ShapePolygon.h"
#include <QScrollArea>
#include <QLabel>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PropertyPanel::PropertyPanel(QWidget *parent)
    : QWidget(parent), m_drawingArea(nullptr), m_currentShape(nullptr)
{
    setupUI();
    setupConnections();
    setupPageStyleConnections();
}

PropertyPanel::~PropertyPanel()
{
}

void PropertyPanel::setDrawingArea(DrawingArea *drawingArea)
{
    m_drawingArea = drawingArea;

    // 设置DrawingArea后，建立页面样式标签页的连接
    setupPageStyleConnections();

    // 连接DrawingArea的信号到PropertyPanel的槽函数
    if (m_drawingArea)
    {
        connect(m_drawingArea, &DrawingArea::backgroundColorChanged,
                this, &PropertyPanel::updateBackgroundColorUI);
        connect(m_drawingArea, &DrawingArea::gridVisibilityChanged,
                this, &PropertyPanel::updateGridVisibilityUI);
        connect(m_drawingArea, &DrawingArea::gridSizeChanged,
                this, &PropertyPanel::updateGridSizeUI);
        connect(m_drawingArea, &DrawingArea::pageSizeChanged,
                this, &PropertyPanel::updatePageSizeUI);
    }
}

void PropertyPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建选项卡控件
    m_tabWidget = new QTabWidget(this);

    // 创建"页面样式"选项卡
    m_pageStyleTab = new QWidget();
    setupPageStyleTab();

    // 创建"图形样式"选项卡
    m_shapeStyleTab = new QWidget();
    setupShapeStyleTab();

    // 添加选项卡到选项卡控件
    m_tabWidget->addTab(m_pageStyleTab, tr("Page Style"));
    m_tabWidget->addTab(m_shapeStyleTab, tr("Shape Style"));

    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(m_tabWidget);

    mainLayout->addWidget(scrollArea);

    // 默认禁用属性面板
    clearProperties();
}

void PropertyPanel::setupPageStyleTab()
{
    QVBoxLayout *layout = new QVBoxLayout(m_pageStyleTab);
    layout->setSpacing(10);

    // 背景设置
    m_backgroundGroup = new QGroupBox(tr("Background"), m_pageStyleTab);
    QGridLayout *backgroundLayout = new QGridLayout(m_backgroundGroup);

    // 背景颜色
    backgroundLayout->addWidget(new QLabel(tr("Color:")), 0, 0);
    m_bgColorButton = new QPushButton();
    m_bgColor = Qt::white; // 默认白色背景
    updateButtonStyle(m_bgColorButton, m_bgColor);
    backgroundLayout->addWidget(m_bgColorButton, 0, 1);

    layout->addWidget(m_backgroundGroup);

    // 尺寸设置
    m_sizeGroup = new QGroupBox(tr("Size"), m_pageStyleTab);
    QGridLayout *sizeLayout = new QGridLayout(m_sizeGroup);

    // 尺寸类型选择
    sizeLayout->addWidget(new QLabel(""), 0, 0); // 占位
    m_sizeTypeCombo = new QComboBox();
    m_sizeTypeCombo->addItem(tr("Custom Size (1215px*614px)"));
    m_sizeTypeCombo->addItem(tr("A3 Size (1500px*2100px)"));
    m_sizeTypeCombo->addItem(tr("A4 Size (1050px*1500px)"));
    m_sizeTypeCombo->addItem(tr("A5 Size (750px*1050px)"));
    sizeLayout->addWidget(m_sizeTypeCombo, 0, 1, 1, 3);

    // 宽度和高度输入框
    QHBoxLayout *sizeDimensionsLayout = new QHBoxLayout();

    m_pageSizeWidthSpinBox = new QSpinBox();
    m_pageSizeWidthSpinBox->setRange(100, 5000);
    m_pageSizeWidthSpinBox->setValue(1215);
    m_pageSizeWidthSpinBox->setSuffix("px");
    QLabel *widthLabel = new QLabel(tr("Width"));

    // 添加交换按钮
    QToolButton *swapDimensionsButton = new QToolButton();
    swapDimensionsButton->setText("⇄");
    swapDimensionsButton->setToolTip(tr("Swap Width/Height"));

    m_pageSizeHeightSpinBox = new QSpinBox();
    m_pageSizeHeightSpinBox->setRange(100, 5000);
    m_pageSizeHeightSpinBox->setValue(614);
    m_pageSizeHeightSpinBox->setSuffix("px");
    QLabel *heightLabel = new QLabel(tr("Height"));

    sizeDimensionsLayout->addWidget(m_pageSizeWidthSpinBox);
    sizeDimensionsLayout->addWidget(widthLabel);
    sizeDimensionsLayout->addWidget(swapDimensionsButton);
    sizeDimensionsLayout->addWidget(m_pageSizeHeightSpinBox);
    sizeDimensionsLayout->addWidget(heightLabel);

    sizeLayout->addLayout(sizeDimensionsLayout, 1, 0, 1, 4);

    layout->addWidget(m_sizeGroup);

    // 网格设置
    m_gridGroup = new QGroupBox(tr("Grid"), m_pageStyleTab);
    QGridLayout *gridLayout = new QGridLayout(m_gridGroup);

    // 显示网格复选框
    m_showGridCheckBox = new QCheckBox(tr("Show Grid"));
    m_showGridCheckBox->setChecked(true); // 默认选中
    gridLayout->addWidget(m_showGridCheckBox, 0, 0, 1, 2);

    // 网格大小
    gridLayout->addWidget(new QLabel(tr("Grid Size:")), 1, 0);
    m_gridSizeCombo = new QComboBox();
    m_gridSizeCombo->addItem(tr("Normal"));
    m_gridSizeCombo->addItem(tr("Large"));
    m_gridSizeCombo->addItem(tr("Small"));
    gridLayout->addWidget(m_gridSizeCombo, 1, 1);

    // 网格尺寸
    gridLayout->addWidget(new QLabel(tr("Grid Size:")), 2, 0);
    m_gridSizeSpinBox = new QSpinBox();
    m_gridSizeSpinBox->setRange(5, 100);
    m_gridSizeSpinBox->setValue(15);
    m_gridSizeSpinBox->setSuffix("px");
    gridLayout->addWidget(m_gridSizeSpinBox, 2, 1);

    layout->addWidget(m_gridGroup);

    // 添加弹性空间
    layout->addStretch();
}

void PropertyPanel::setupShapeStyleTab()
{
    QVBoxLayout *layout = new QVBoxLayout(m_shapeStyleTab);

    // 图形类型和不透明度
    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel(tr("Type:")));
    // 使用标签替代下拉框，只用于展示当前类型
    m_shapeTypeLabel = new QLabel(tr("Rectangle"));
    typeLayout->addWidget(m_shapeTypeLabel);
    layout->addLayout(typeLayout);

    QHBoxLayout *opacityLayout = new QHBoxLayout();
    opacityLayout->addWidget(new QLabel(tr("Opacity:")));
    m_opacitySpinBox = new QDoubleSpinBox();
    m_opacitySpinBox->setMinimum(0);
    m_opacitySpinBox->setMaximum(100);
    m_opacitySpinBox->setValue(100);
    m_opacitySpinBox->setSuffix("%");
    opacityLayout->addWidget(m_opacitySpinBox);
    layout->addLayout(opacityLayout);

    // 布局部分
    m_layoutGroup = new QGroupBox(tr("Layout"), m_shapeStyleTab);
    QGridLayout *layoutGroupLayout = new QGridLayout(m_layoutGroup);

    layoutGroupLayout->addWidget(new QLabel(tr("Width:")), 0, 0);
    m_widthSpinBox = new QSpinBox();
    m_widthSpinBox->setRange(1, 9999);
    m_widthSpinBox->setSuffix("px");
    layoutGroupLayout->addWidget(m_widthSpinBox, 0, 1);

    layoutGroupLayout->addWidget(new QLabel(tr("Height:")), 0, 2);
    m_heightSpinBox = new QSpinBox();
    m_heightSpinBox->setRange(1, 9999);
    m_heightSpinBox->setSuffix("px");
    layoutGroupLayout->addWidget(m_heightSpinBox, 0, 3);

    layoutGroupLayout->addWidget(new QLabel("X:"), 1, 0);
    m_xPosSpinBox = new QSpinBox();
    m_xPosSpinBox->setRange(0, 9999);
    m_xPosSpinBox->setSuffix("px");
    layoutGroupLayout->addWidget(m_xPosSpinBox, 1, 1);

    layoutGroupLayout->addWidget(new QLabel("Y:"), 1, 2);
    m_yPosSpinBox = new QSpinBox();
    m_yPosSpinBox->setRange(0, 9999);
    m_yPosSpinBox->setSuffix("px");
    layoutGroupLayout->addWidget(m_yPosSpinBox, 1, 3);

    layoutGroupLayout->addWidget(new QLabel(tr("Angle:")), 2, 0);
    m_rotationSpinBox = new QDoubleSpinBox();
    m_rotationSpinBox->setRange(0, 360);
    m_rotationSpinBox->setSuffix("°");
    layoutGroupLayout->addWidget(m_rotationSpinBox, 2, 1);

    // 添加旋转按钮
    QHBoxLayout *rotationBtnsLayout = new QHBoxLayout();
    m_rotateLeftBtn = new QToolButton();
    m_rotateLeftBtn->setText("←");
    m_rotateLeftBtn->setToolTip(tr("Rotate Left"));

    m_vFlipButton = new QToolButton();
    m_vFlipButton->setText("↕");
    m_vFlipButton->setToolTip(tr("Vertical Flip (Set angle to 0)"));

    m_hFlipButton = new QToolButton();
    m_hFlipButton->setText("↔");
    m_hFlipButton->setToolTip(tr("Horizontal Flip (Set angle to 90)"));

    m_rotateRightBtn = new QToolButton();
    m_rotateRightBtn->setText("→");
    m_rotateRightBtn->setToolTip(tr("Rotate Right"));

    rotationBtnsLayout->addWidget(m_rotateLeftBtn);
    rotationBtnsLayout->addWidget(m_vFlipButton);
    rotationBtnsLayout->addWidget(m_hFlipButton);
    rotationBtnsLayout->addWidget(m_rotateRightBtn);

    layoutGroupLayout->addLayout(rotationBtnsLayout, 2, 2, 1, 2);

    layout->addWidget(m_layoutGroup);

    // 字体部分
    m_fontGroup = new QGroupBox(tr("Font and Size"), m_shapeStyleTab);
    QGridLayout *fontGroupLayout = new QGridLayout(m_fontGroup);

    fontGroupLayout->addWidget(new QLabel(tr("Font:")), 0, 0);
    m_fontFamilyCombo = new QComboBox();
    m_fontFamilyCombo->addItem("Microsoft YaHei");
    m_fontFamilyCombo->addItem("SimSun");
    m_fontFamilyCombo->addItem("Arial");
    fontGroupLayout->addWidget(m_fontFamilyCombo, 0, 1, 1, 3);

    // 文字颜色按钮
    QHBoxLayout *textColorLayout = new QHBoxLayout();
    textColorLayout->addWidget(new QLabel(tr("Text Color:")));
    m_textColorButton = new QPushButton();
    m_textColor = Qt::black; // 默认黑色
    updateButtonStyle(m_textColorButton, m_textColor);
    textColorLayout->addWidget(m_textColorButton);

    // 字体样式
    m_fontSizeSpinBox = new QSpinBox();
    m_fontSizeSpinBox->setRange(8, 72);
    m_fontSizeSpinBox->setValue(12);
    m_fontSizeSpinBox->setSuffix("px");
    fontGroupLayout->addWidget(m_fontSizeSpinBox, 1, 0, 1, 2);

    m_lineHeightSpinBox = new QDoubleSpinBox();
    m_lineHeightSpinBox->setRange(0.5, 3.0);
    m_lineHeightSpinBox->setValue(1.25);
    fontGroupLayout->addWidget(m_lineHeightSpinBox, 1, 2, 1, 2);

    // 对齐方式下拉框
    QHBoxLayout *alignLayout = new QHBoxLayout();

    // 水平对齐方式
    alignLayout->addWidget(new QLabel(tr("Horizontal Align:")));
    m_hAlignCombo = new QComboBox();
    m_hAlignCombo->addItem(tr("Left"));
    m_hAlignCombo->addItem(tr("Center"));
    m_hAlignCombo->addItem(tr("Right"));
    alignLayout->addWidget(m_hAlignCombo);

    // 垂直对齐方式
    alignLayout->addWidget(new QLabel(tr("Vertical Align:")));
    m_vAlignCombo = new QComboBox();
    m_vAlignCombo->addItem(tr("Top"));
    m_vAlignCombo->addItem(tr("Middle"));
    m_vAlignCombo->addItem(tr("Bottom"));
    alignLayout->addWidget(m_vAlignCombo);

    fontGroupLayout->addLayout(alignLayout, 2, 0, 1, 4);

    // 文本样式按钮
    QHBoxLayout *textStyleLayout = new QHBoxLayout();

    m_boldButton = new QToolButton();
    m_boldButton->setText("B");
    m_boldButton->setCheckable(true);
    m_boldButton->setToolTip(tr("Bold"));

    m_italicButton = new QToolButton();
    m_italicButton->setText("I");
    m_italicButton->setCheckable(true);
    m_italicButton->setToolTip(tr("Italic"));

    m_underlineButton = new QToolButton();
    m_underlineButton->setText("U");
    m_underlineButton->setCheckable(true);
    m_underlineButton->setToolTip(tr("Underline"));

    m_strikeoutButton = new QToolButton();
    m_strikeoutButton->setText("S");
    m_strikeoutButton->setCheckable(true);
    m_strikeoutButton->setToolTip(tr("Strikethrough"));

    textStyleLayout->addWidget(m_boldButton);
    textStyleLayout->addWidget(m_italicButton);
    textStyleLayout->addWidget(m_underlineButton);
    textStyleLayout->addWidget(m_strikeoutButton);

    // 添加文本样式和文字颜色布局到字体分组
    fontGroupLayout->addLayout(textStyleLayout, 3, 0, 1, 4);
    fontGroupLayout->addLayout(textColorLayout, 4, 0, 1, 4);

    layout->addWidget(m_fontGroup);

    // 填充部分
    m_fillGroup = new QGroupBox(tr("Fill"), m_shapeStyleTab);
    QGridLayout *fillGroupLayout = new QGridLayout(m_fillGroup);

    m_fillTypeCombo = new QComboBox();
    m_fillTypeCombo->addItem(tr("Solid Color"));
    m_fillTypeCombo->addItem(tr("No Fill"));
    m_fillTypeCombo->addItem(tr("Gradient"));
    fillGroupLayout->addWidget(m_fillTypeCombo, 0, 0, 1, 2);

    m_fillColorButton = new QPushButton();
    m_fillColor = Qt::white;
    updateButtonStyle(m_fillColorButton, m_fillColor);
    fillGroupLayout->addWidget(m_fillColorButton, 0, 2, 1, 2);

    layout->addWidget(m_fillGroup);

    // 线条部分
    m_lineGroup = new QGroupBox(tr("Line"), m_shapeStyleTab);
    QGridLayout *lineGroupLayout = new QGridLayout(m_lineGroup);

    lineGroupLayout->addWidget(new QLabel(tr("Line Color:")), 0, 0);
    m_lineColorButton = new QPushButton();
    m_lineColor = Qt::black;
    updateButtonStyle(m_lineColorButton, m_lineColor);
    lineGroupLayout->addWidget(m_lineColorButton, 0, 1, 1, 3);

    lineGroupLayout->addWidget(new QLabel(tr("Line Type:")), 1, 0);
    m_lineTypeCombo = new QComboBox();
    m_lineTypeCombo->addItem(tr("Solid"));
    m_lineTypeCombo->addItem(tr("Dashed"));
    m_lineTypeCombo->addItem(tr("Dotted"));
    lineGroupLayout->addWidget(m_lineTypeCombo, 1, 1, 1, 3);

    lineGroupLayout->addWidget(new QLabel(tr("Line Width:")), 2, 0);
    m_lineWidthSpinBox = new QDoubleSpinBox();
    m_lineWidthSpinBox->setRange(0.5, 10.0);
    m_lineWidthSpinBox->setValue(1.5);
    m_lineWidthSpinBox->setSuffix("px");
    lineGroupLayout->addWidget(m_lineWidthSpinBox, 2, 1, 1, 3);

    // 箭头选项（仅用于箭头图形）
    lineGroupLayout->addWidget(new QLabel(tr("Arrow Type:")), 3, 0);
    m_startArrowCombo = new QComboBox();
    m_startArrowCombo->addItem(tr("None"));
    m_startArrowCombo->addItem(tr("Standard"));
    m_startArrowCombo->addItem(tr("Triangle"));
    lineGroupLayout->addWidget(m_startArrowCombo, 3, 1);

    m_endArrowCombo = new QComboBox();
    m_endArrowCombo->addItem(tr("None"));
    m_endArrowCombo->addItem(tr("Standard"));
    m_endArrowCombo->addItem(tr("Triangle"));
    lineGroupLayout->addWidget(m_endArrowCombo, 3, 2);

    // 连接线类型
    lineGroupLayout->addWidget(new QLabel(tr("Connection Type:")), 4, 0);
    m_connectionTypeCombo = new QComboBox();
    m_connectionTypeCombo->addItem(tr("Straight"));
    m_connectionTypeCombo->addItem(tr("Segmented"));
    m_connectionTypeCombo->addItem(tr("Curved"));
    lineGroupLayout->addWidget(m_connectionTypeCombo, 4, 1, 1, 3);

    layout->addWidget(m_lineGroup);
    layout->addStretch();
}

void PropertyPanel::setupConnections()
{
    // 连接宽度和高度的变化信号
    connect(m_widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int width)
            {
        if (m_currentShape && m_drawingArea) {
            QRect rect = m_currentShape->getRect();
            rect.setWidth(width);
            m_currentShape->resize(rect);
            m_drawingArea->update();
        } });

    connect(m_heightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int height)
            {
        if (m_currentShape && m_drawingArea) {
            QRect rect = m_currentShape->getRect();
            rect.setHeight(height);
            m_currentShape->resize(rect);
            m_drawingArea->update();
        } });

    // 连接X和Y位置的变化信号
    connect(m_xPosSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int x)
            {
        if (m_currentShape && m_drawingArea) {
            QRect rect = m_currentShape->getRect();
            rect.moveLeft(x);
            m_currentShape->resize(rect);
            m_drawingArea->update();
        } });

    connect(m_yPosSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int y)
            {
        if (m_currentShape && m_drawingArea) {
            QRect rect = m_currentShape->getRect();
            rect.moveTop(y);
            m_currentShape->resize(rect);
            m_drawingArea->update();
        } });

    // 连接不透明度的变化信号
    connect(m_opacitySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double opacity)
            {
        if (m_currentShape && m_drawingArea) {
            m_currentShape->setOpacity(opacity / 100.0); // 将百分比转换为0-1的范围
            m_drawingArea->update();
        } });

    // 连接旋转角度的变化信号
    connect(m_rotationSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double angle)
            {
        if (m_currentShape && m_drawingArea) {
            m_currentShape->setRotation(angle * (M_PI / 180.0));
            m_drawingArea->update();
        } });

    // 连接垂直翻转按钮
    connect(m_vFlipButton, &QToolButton::clicked, this, [this]()
            {
        if (m_currentShape && m_drawingArea) {
            m_rotationSpinBox->setValue(0);  // 设置角度为0
            m_currentShape->setRotation(0);
            m_drawingArea->update();
        } });

    // 连接水平翻转按钮
    connect(m_hFlipButton, &QToolButton::clicked, this, [this]()
            {
        if (m_currentShape && m_drawingArea) {
            m_rotationSpinBox->setValue(90);  // 设置角度为90
            m_currentShape->setRotation(90 * (M_PI / 180.0));
            m_drawingArea->update();
        } });

    // 连接向左旋转按钮
    connect(m_rotateLeftBtn, &QToolButton::clicked, this, [this]()
            {
        if (m_currentShape && m_drawingArea) {
            // 获取当前角度，逆时针旋转15度
            double currentAngle = m_rotationSpinBox->value();
            double newAngle = fmod(currentAngle - 15 + 360, 360);  // 确保角度在0-360范围内
            m_rotationSpinBox->setValue(newAngle);
        } });

    // 连接向右旋转按钮
    connect(m_rotateRightBtn, &QToolButton::clicked, this, [this]()
            {
        if (m_currentShape && m_drawingArea) {
            // 获取当前角度，顺时针旋转15度
            double currentAngle = m_rotationSpinBox->value();
            double newAngle = fmod(currentAngle + 15, 360);  // 确保角度在0-360范围内
            m_rotationSpinBox->setValue(newAngle);
        } });

    // 连接线宽的变化信号
    connect(m_lineWidthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double width)
            {
        if (m_currentShape && m_drawingArea) {
            m_currentShape->setLineWidth(width);
            m_drawingArea->update();
        } });

    // 连接填充颜色按钮
    connect(m_fillColorButton, &QPushButton::clicked, this, [this]()
            {
        QColor color = QColorDialog::getColor(m_fillColor, this, tr("Select Fill Color"));
        if (color.isValid()) {
            m_fillColor = color;
            updateButtonStyle(m_fillColorButton, color);
            if (m_currentShape && m_drawingArea) {
                m_currentShape->setFillColor(color);
                m_drawingArea->update();
            }
        } });

    // 连接线条颜色按钮
    connect(m_lineColorButton, &QPushButton::clicked, this, [this]()
            {
        QColor color = QColorDialog::getColor(m_lineColor, this, tr("Select Line Color"));
        if (color.isValid()) {
            m_lineColor = color;
            updateButtonStyle(m_lineColorButton, color);
            if (m_currentShape && m_drawingArea) {
                m_currentShape->setLineColor(color);
                m_drawingArea->update();
            }
        } });

    // 连接线条类型下拉框
    connect(m_lineTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
            {
        if (m_currentShape && m_drawingArea) {
            m_currentShape->setLineType(static_cast<ShapeBase::LineType>(index));
            m_drawingArea->update();
        } });

    // 连接字体下拉框
    connect(m_fontFamilyCombo, &QComboBox::currentTextChanged, this, [this](const QString &family)
            {
        if (m_currentShape && m_drawingArea) {
            m_currentShape->setFontFamily(family);
            m_drawingArea->update();
        } });

    // 连接字体大小
    connect(m_fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int size)
            {
        if (m_currentShape && m_drawingArea) {
            m_currentShape->setFontSize(size);
            m_drawingArea->update();
        } });

    // 连接行高
    connect(m_lineHeightSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double height)
            {
        if (m_currentShape && m_drawingArea && m_currentShape->isTextEditable()) {
            // 设置行高的代码，可能需要在ShapeBase中添加相应方法
            m_drawingArea->update();
        } }); // 连接水平对齐方式下拉框
    connect(m_hAlignCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
            {
        if (m_currentShape && m_drawingArea && m_currentShape->isTextEditable()) {
            updateTextAlignment();
        } });

    // 连接垂直对齐方式下拉框
    connect(m_vAlignCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
            {
        if (m_currentShape && m_drawingArea && m_currentShape->isTextEditable()) {
            updateTextAlignment();
        } });

    // 连接文本样式按钮
    connect(m_boldButton, &QToolButton::toggled, this, [this](bool checked)
            {
        if (m_currentShape && m_drawingArea && m_currentShape->isTextEditable()) {
            m_currentShape->setFontBold(checked);
            m_drawingArea->update();
        } });

    connect(m_italicButton, &QToolButton::toggled, this, [this](bool checked)
            {
        if (m_currentShape && m_drawingArea && m_currentShape->isTextEditable()) {
            m_currentShape->setFontItalic(checked);
            m_drawingArea->update();
        } });

    connect(m_underlineButton, &QToolButton::toggled, this, [this](bool checked)
            {
        if (m_currentShape && m_drawingArea && m_currentShape->isTextEditable()) {
            m_currentShape->setFontUnderline(checked);
            m_drawingArea->update();
        } });

    connect(m_strikeoutButton, &QToolButton::toggled, this, [this](bool checked)
            {
        if (m_currentShape && m_drawingArea && m_currentShape->isTextEditable()) {
            m_currentShape->setFontStrikeOut(checked);
            m_drawingArea->update();
        } });

    // 连接文字颜色按钮
    connect(m_textColorButton, &QPushButton::clicked, this, [this]()
            {
        QColor color = QColorDialog::getColor(m_textColor, this, tr("Select Text Color"));
        if (color.isValid()) {
            m_textColor = color;
            updateButtonStyle(m_textColorButton, color);
            if (m_currentShape && m_drawingArea && m_currentShape->isTextEditable()) {
                m_currentShape->setTextColor(color);
                m_drawingArea->update();
            }
        } });

    // 其他连接可以根据需要添加
}

// 设置页面样式标签页中的事件连接
void PropertyPanel::setupPageStyleConnections()
{
    if (!m_drawingArea)
        return;

    // 背景颜色按钮点击事件
    connect(m_bgColorButton, &QPushButton::clicked, this, [this]()
            {
        QColor color = QColorDialog::getColor(m_bgColor, this, tr("Select Background Color"));
        if (color.isValid() && m_drawingArea) {
            m_bgColor = color;
            updateButtonStyle(m_bgColorButton, color);
            m_drawingArea->setBackgroundColor(color);
        } });

    // 尺寸类型下拉框切换事件
    connect(m_sizeTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
            {
        if (!m_drawingArea)
            return;
            
        // 根据选择的尺寸类型设置页面大小
        switch (index) {
            case 0: // Custom Size
                m_pageSizeWidthSpinBox->setValue(1215);
                m_pageSizeHeightSpinBox->setValue(614);
                break;
            case 1: // A3 Size (1500px*2100px)
                m_pageSizeWidthSpinBox->setValue(1500);
                m_pageSizeHeightSpinBox->setValue(2100);
                break;
            case 2: // A4 Size (1050px*1500px)
                m_pageSizeWidthSpinBox->setValue(1050);
                m_pageSizeHeightSpinBox->setValue(1500);
                break;
            case 3: // A5 Size (750px*1050px)
                m_pageSizeWidthSpinBox->setValue(750);
                m_pageSizeHeightSpinBox->setValue(1050);
                break;
        } });

    // 宽度和高度值变化事件
    connect(m_pageSizeWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int width)
            {
        if (m_drawingArea) {
            QSize newSize(width, m_pageSizeHeightSpinBox->value());
            m_drawingArea->setPageSize(newSize);
        } });

    connect(m_pageSizeHeightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int height)
            {
        if (m_drawingArea) {
            QSize newSize(m_pageSizeWidthSpinBox->value(), height);
            m_drawingArea->setPageSize(newSize);
        } });

    // 交换宽高按钮事件
    QToolButton *swapButton = m_sizeGroup->findChild<QToolButton *>();
    if (swapButton)
    {
        connect(swapButton, &QToolButton::clicked, this, [this]()
                {
            int width = m_pageSizeWidthSpinBox->value();
            int height = m_pageSizeHeightSpinBox->value();
            m_pageSizeWidthSpinBox->setValue(height);
            m_pageSizeHeightSpinBox->setValue(width); });
    }

    // 显示网格复选框事件
    connect(m_showGridCheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
        if (m_drawingArea) {
            m_drawingArea->setGridVisible(checked);
            
            // 如果网格被关闭，禁用网格设置选项，否则启用
            m_gridSizeCombo->setEnabled(checked);
            m_gridSizeSpinBox->setEnabled(checked);
        } });

    // 网格大小下拉框事件
    connect(m_gridSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
            {
        if (!m_drawingArea)
            return;
            
        // 根据选择的网格大小设置网格尺寸
        int size;
        switch (index) {
            case 0: // Normal
                size = 15;
                break;
            case 1: // Large
                size = 25;
                break;
            case 2: // Small
                size = 8;
                break;
            default:
                size = 15;
                break;
        }
        
        // 阻塞信号以防止循环
        m_gridSizeSpinBox->blockSignals(true);
        m_gridSizeSpinBox->setValue(size);
        m_gridSizeSpinBox->blockSignals(false);
        
        // 通过DrawingArea更新网格大小
        m_drawingArea->setGridSize(size); });

    // 网格尺寸值变化事件
    connect(m_gridSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int size)
            {
        if (m_drawingArea) {
            m_drawingArea->setGridSize(size);
            
            // 更新网格大小下拉框选项
            m_gridSizeCombo->blockSignals(true);
            if (size == 15)
                m_gridSizeCombo->setCurrentIndex(0);
            else if (size == 25)
                m_gridSizeCombo->setCurrentIndex(1);
            else if (size == 8)
                m_gridSizeCombo->setCurrentIndex(2);
            m_gridSizeCombo->blockSignals(false);
        } });
}

void PropertyPanel::updateForSelectedShape(ShapeBase *shape)
{
    if (!shape)
    {
        clearProperties();
        return;
    }

    m_currentShape = shape;
    m_tabWidget->setCurrentWidget(m_shapeStyleTab);

    // 启用所有控件
    m_shapeStyleTab->setEnabled(true);

    // 更新属性面板中的值
    updateShapeProperties(shape);
}

void PropertyPanel::updateShapeProperties(ShapeBase *shape)
{
    if (!shape)
        return;

    // 根据图形类型更新图形类型标签
    if (dynamic_cast<ShapeRect *>(shape))
    {
        m_shapeTypeLabel->setText(tr("Rectangle"));
    }
    else if (dynamic_cast<ShapeEllipse *>(shape))
    {
        m_shapeTypeLabel->setText(tr("Ellipse"));
    }
    else if (dynamic_cast<ShapeArrow *>(shape))
    {
        m_shapeTypeLabel->setText(tr("Arrow"));
    }
    else if (dynamic_cast<ShapePolygon *>(shape))
    {
        m_shapeTypeLabel->setText(tr("Polygon"));
    }

    // 更新宽度、高度和位置
    QRect rect = shape->getRect();
    m_widthSpinBox->setValue(rect.width());
    m_heightSpinBox->setValue(rect.height());
    m_xPosSpinBox->setValue(rect.x());
    m_yPosSpinBox->setValue(rect.y());

    // 更新旋转角度
    m_rotationSpinBox->setValue(shape->getRotation() * (180.0 / M_PI));

    // 更新不透明度
    m_opacitySpinBox->setValue(shape->getOpacity() * 100.0); // 转换为百分比

    // 更新线宽
    m_lineWidthSpinBox->setValue(shape->getLineWidth());

    // 更新线条类型
    m_lineTypeCombo->setCurrentIndex(static_cast<int>(shape->getLineType()));

    // 更新颜色
    m_lineColor = shape->getLineColor();
    updateButtonStyle(m_lineColorButton, m_lineColor);

    m_fillColor = shape->getFillColor();
    updateButtonStyle(m_fillColorButton, m_fillColor);

    // 更新文本（如果图形支持文本）
    if (shape->isTextEditable())
    {
        m_fontGroup->setEnabled(true);

        // 更新字体相关属性
        QFont font = shape->getFont();
        if (!font.family().isEmpty())
        {
            int index = m_fontFamilyCombo->findText(font.family());
            if (index >= 0)
            {
                m_fontFamilyCombo->setCurrentIndex(index);
            }
        }

        // 更新文字颜色
        m_textColor = shape->getTextColor();
        updateButtonStyle(m_textColorButton, m_textColor);

        if (font.pointSize() > 0)
        {
            m_fontSizeSpinBox->setValue(font.pointSize());
        }

        // 更新文本对齐方式
        int alignment = shape->getTextAlignment();

        // 水平对齐
        if (alignment & Qt::AlignLeft)
            m_hAlignCombo->setCurrentIndex(0);
        else if (alignment & Qt::AlignRight)
            m_hAlignCombo->setCurrentIndex(2);
        else
            m_hAlignCombo->setCurrentIndex(1); // 默认居中

        // 垂直对齐
        if (alignment & Qt::AlignTop)
            m_vAlignCombo->setCurrentIndex(0);
        else if (alignment & Qt::AlignBottom)
            m_vAlignCombo->setCurrentIndex(2);
        else
            m_vAlignCombo->setCurrentIndex(1); // 默认居中

        // 更新字体样式按钮状态
        m_boldButton->setChecked(shape->isFontBold());
        m_italicButton->setChecked(shape->isFontItalic());
        m_underlineButton->setChecked(shape->isFontUnderline());
        m_strikeoutButton->setChecked(shape->isFontStrikeOut());
    }
    else
    {
        m_fontGroup->setEnabled(false);
    }

    // 根据图形类型启用/禁用特定控件
    bool isArrow = dynamic_cast<ShapeArrow *>(shape);
    m_startArrowCombo->setEnabled(isArrow);
    m_endArrowCombo->setEnabled(isArrow);
    m_connectionTypeCombo->setEnabled(isArrow);
}

void PropertyPanel::clearProperties()
{
    m_currentShape = nullptr;
    m_shapeStyleTab->setEnabled(false);
}

void PropertyPanel::updateButtonStyle(QPushButton *button, const QColor &color)
{
    if (!button)
        return;

    // 检测是否为透明色
    if (color.alpha() < 255)
    {
        // 对于透明或半透明的颜色，创建一个棋盘格背景来显示透明度
        QString stylesheet = QString(
                                 "QPushButton {"
                                 "  background-color: %1;"
                                 "  border: 1px solid darkgray;"
                                 "  border-radius: 3px;"
                                 "}")
                                 .arg(color.name(QColor::HexArgb)); // 使用带透明度的格式
        button->setStyleSheet(stylesheet);
    }
    else
    {
        // 对于不透明的颜色，使用纯色背景
        QString stylesheet = QString(
                                 "QPushButton {"
                                 "  background-color: %1;"
                                 "  border: 1px solid darkgray;"
                                 "  border-radius: 3px;"
                                 "}")
                                 .arg(color.name());
        button->setStyleSheet(stylesheet);
    }

    // 设置工具提示，显示RGB值
    button->setToolTip(QString("RGB: %1, %2, %3").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void PropertyPanel::updateTextAlignment()
{
    if (!m_currentShape || !m_drawingArea)
        return;

    // 获取水平对齐方式
    int hAlign;
    switch (m_hAlignCombo->currentIndex())
    {
    case 0:
        hAlign = Qt::AlignLeft;
        break;
    case 2:
        hAlign = Qt::AlignRight;
        break;
    default:
        hAlign = Qt::AlignHCenter;
        break;
    }

    // 获取垂直对齐方式
    int vAlign;
    switch (m_vAlignCombo->currentIndex())
    {
    case 0:
        vAlign = Qt::AlignTop;
        break;
    case 2:
        vAlign = Qt::AlignBottom;
        break;
    default:
        vAlign = Qt::AlignVCenter;
        break;
    }

    // 组合对齐方式
    int alignment = hAlign | vAlign;

    // 应用到当前形状
    m_currentShape->setTextAlignment(alignment);
    m_drawingArea->update();
}

// 更新背景颜色UI - 用于外部同步
void PropertyPanel::updateBackgroundColorUI(const QColor &color)
{
    if (m_bgColor != color)
    {
        m_bgColor = color;
        updateButtonStyle(m_bgColorButton, color);
    }
}

// 更新网格可见性UI - 用于外部同步
void PropertyPanel::updateGridVisibilityUI(bool visible)
{
    if (m_showGridCheckBox->isChecked() != visible)
    {
        m_showGridCheckBox->blockSignals(true);
        m_showGridCheckBox->setChecked(visible);
        m_gridSizeCombo->setEnabled(visible);
        m_gridSizeSpinBox->setEnabled(visible);
        m_showGridCheckBox->blockSignals(false);
    }
}

// 更新网格大小UI - 用于外部同步
void PropertyPanel::updateGridSizeUI(int size)
{
    if (m_gridSizeSpinBox->value() != size)
    {
        // 阻塞信号以防止循环更新
        m_gridSizeSpinBox->blockSignals(true);
        m_gridSizeSpinBox->setValue(size);

        // 更新网格大小下拉框
        m_gridSizeCombo->blockSignals(true);
        if (size == 15)
            m_gridSizeCombo->setCurrentIndex(0); // Normal
        else if (size == 25)
            m_gridSizeCombo->setCurrentIndex(1); // Large
        else if (size == 8)
            m_gridSizeCombo->setCurrentIndex(2); // Small
        else
        {
            // 对于其他尺寸，保持当前选择状态
        }
        m_gridSizeCombo->blockSignals(false);

        m_gridSizeSpinBox->blockSignals(false);
    }
}

// 更新页面大小UI - 用于外部同步
void PropertyPanel::updatePageSizeUI(const QSize &size)
{
    // 阻塞信号以防止循环更新
    m_pageSizeWidthSpinBox->blockSignals(true);
    m_pageSizeHeightSpinBox->blockSignals(true);
    m_sizeTypeCombo->blockSignals(true);

    m_pageSizeWidthSpinBox->setValue(size.width());
    m_pageSizeHeightSpinBox->setValue(size.height());

    // 检查大小是否匹配预设尺寸
    if (size == QSize(1215, 614))
        m_sizeTypeCombo->setCurrentIndex(0);
    else if (size == QSize(1500, 2100))
        m_sizeTypeCombo->setCurrentIndex(1);
    else if (size == QSize(1050, 1500))
        m_sizeTypeCombo->setCurrentIndex(2);
    else if (size == QSize(750, 1050))
        m_sizeTypeCombo->setCurrentIndex(3);
    // 如果不匹配任何预设尺寸，保持当前选择状态

    m_pageSizeWidthSpinBox->blockSignals(false);
    m_pageSizeHeightSpinBox->blockSignals(false);
    m_sizeTypeCombo->blockSignals(false);
}
