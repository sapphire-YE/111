#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QColorDialog>
#include <QButtonGroup>
#include <QToolButton>
#include <QCheckBox>
#include <cmath>
#include "ShapeBase.h"

class DrawingArea;

class PropertyPanel : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyPanel(QWidget *parent = nullptr);
    ~PropertyPanel();

    void setDrawingArea(DrawingArea *drawingArea);
    void updateForSelectedShape(ShapeBase *shape);
    void clearProperties();

    // UI更新方法 - 用于同步外部修改
    void updateBackgroundColorUI(const QColor &color);
    void updateGridVisibilityUI(bool visible);
    void updateGridSizeUI(int size);
    void updatePageSizeUI(const QSize &size);

signals:
    void propertyChanged();

private:
    void setupUI();
    void setupPageStyleTab();
    void setupShapeStyleTab();
    void setupConnections();
    void setupPageStyleConnections(); // 设置页面样式标签页中的事件连接
    void updateShapeProperties(ShapeBase *shape);
    void updateButtonStyle(QPushButton *button, const QColor &color);
    void updateTextAlignment(); // 更新文本对齐方式

    DrawingArea *m_drawingArea;
    ShapeBase *m_currentShape;

    // UI Elements
    QTabWidget *m_tabWidget;

    // 页面样式标签页
    QWidget *m_pageStyleTab;

    // 背景设置
    QGroupBox *m_backgroundGroup;
    QPushButton *m_bgColorButton;
    QColor m_bgColor;

    // 尺寸设置
    QGroupBox *m_sizeGroup;
    QComboBox *m_sizeTypeCombo;
    QSpinBox *m_pageSizeWidthSpinBox;  // 页面宽度
    QSpinBox *m_pageSizeHeightSpinBox; // 页面高度

    // 网格设置
    QGroupBox *m_gridGroup;
    QCheckBox *m_showGridCheckBox; // 显示网格
    QComboBox *m_gridSizeCombo;    // 网格大小
    QSpinBox *m_gridSizeSpinBox;   // 网格尺寸

    // 图形样式标签页
    QWidget *m_shapeStyleTab;
    QLabel *m_shapeTypeLabel; // 使用标签替代下拉框
    QDoubleSpinBox *m_opacitySpinBox;

    // 布局部分
    QGroupBox *m_layoutGroup;
    QSpinBox *m_widthSpinBox;
    QSpinBox *m_heightSpinBox;
    QSpinBox *m_xPosSpinBox;
    QSpinBox *m_yPosSpinBox;
    QDoubleSpinBox *m_rotationSpinBox;
    QToolButton *m_vFlipButton;    // 垂直翻转按钮
    QToolButton *m_hFlipButton;    // 水平翻转按钮
    QToolButton *m_rotateLeftBtn;  // 向左旋转按钮
    QToolButton *m_rotateRightBtn; // 向右旋转按钮

    // 字体部分
    QGroupBox *m_fontGroup;
    QComboBox *m_fontFamilyCombo;
    QComboBox *m_fontStyleCombo;
    QSpinBox *m_fontSizeSpinBox;
    QDoubleSpinBox *m_lineHeightSpinBox;
    QComboBox *m_hAlignCombo;       // 水平对齐方式
    QComboBox *m_vAlignCombo;       // 垂直对齐方式
    QToolButton *m_boldButton;      // 加粗按钮
    QToolButton *m_italicButton;    // 斜体按钮
    QToolButton *m_underlineButton; // 下划线按钮
    QToolButton *m_strikeoutButton; // 中划线按钮
    QPushButton *m_textColorButton; // 文字颜色按钮
    QColor m_textColor;             // 文字颜色

    // 填充部分
    QGroupBox *m_fillGroup;
    QComboBox *m_fillTypeCombo;
    QPushButton *m_fillColorButton;
    QColor m_fillColor;

    // 线条部分
    QGroupBox *m_lineGroup;
    QPushButton *m_lineColorButton;
    QColor m_lineColor;
    QComboBox *m_lineTypeCombo;
    QDoubleSpinBox *m_lineWidthSpinBox;
    QComboBox *m_startArrowCombo;
    QComboBox *m_endArrowCombo;
    QComboBox *m_connectionTypeCombo;
};

#endif // PROPERTYPANEL_H
