#include "ColorPopupWidget.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegExpValidator>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QSettings>
#include <QDebug>

// 定义静态成员变量
QVector<QColor> ColorPopupWidget::s_recentColors;
const int ColorPopupWidget::MAX_RECENT_COLORS;

ColorPopupWidget::ColorPopupWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Popup);
    setFixedWidth(320);


    m_systemColors = {

        QColor("#FFFFFF"), QColor("#F8F8F8"), QColor("#EDEDED"), QColor("#D9D9D9"), QColor("#BFBFBF"), QColor("#8C8C8C"),
        QColor("#595959"), QColor("#262626"), QColor("#000000"), QColor("#FFCCC7"), QColor("#FFA39E"), QColor("#FF7875"),
        QColor("#FF4D4F"), QColor("#F5222D"), QColor("#CF1322"), QColor("#A8071A"), QColor("#820014"), QColor("#5C0011"),
        QColor("#FFF1B8"), QColor("#FFE58F"), QColor("#FFD666"), QColor("#FFC53D"), QColor("#FAAD14"), QColor("#D48806"),
        QColor("#AD6800"), QColor("#874D00"), QColor("#613400"), QColor("#FAFA8C"), QColor("#FFFF00"), QColor("#FFFB8F"),
        QColor("#FFF566"), QColor("#FFEC3D"), QColor("#FADB14"), QColor("#D4B106"), QColor("#AD8B00"), QColor("#876800"),
        QColor("#613400"), QColor("#D3F261"), QColor("#B7EB8F"), QColor("#95DE64"), QColor("#73D13D"), QColor("#52C41A"),
        QColor("#389E0D"), QColor("#237804"), QColor("#135200"), QColor("#092B00"), QColor("#91D5FF"), QColor("#69C0FF"),
        QColor("#40A9FF"), QColor("#1890FF"), QColor("#096DD9"), QColor("#0050B3"), QColor("#003A8C"), QColor("#002766"),
        QColor("#F759AB"), QColor("#EB2F96"), QColor("#C41D7F"), QColor("#9E1068"), QColor("#780650")
    };

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // 系统色块标题
    QLabel* sysLabel = new QLabel(tr("System Colors"), this);
    mainLayout->addWidget(sysLabel);

    // 色块网格
    QGridLayout* grid = new QGridLayout;
    grid->setSpacing(4);
    int cols = 9;
    for (int i = 0; i < m_systemColors.size(); ++i) {
        QPushButton* btn = new QPushButton(this);
        btn->setFixedSize(24, 24);
        btn->setStyleSheet(QString("background:%1; border:1px solid #ccc;").arg(m_systemColors[i].name()));
        btn->setProperty("color", m_systemColors[i]);
        connect(btn, &QPushButton::clicked, this, &ColorPopupWidget::onColorBlockClicked);
        grid->addWidget(btn, i / cols, i % cols);
    }
    mainLayout->addLayout(grid);

    // 最近使用
    QLabel* recentLabel = new QLabel(tr("Recent Colors"), this);
    mainLayout->addWidget(recentLabel);

    QHBoxLayout* recentLayout = new QHBoxLayout;
    for (int i = 0; i < 8; ++i) {
        QPushButton* btn = new QPushButton(this);
        btn->setFixedSize(24, 24);
        if (i < s_recentColors.size()) {
            btn->setStyleSheet(QString("background:%1; border:1px solid #ccc;").arg(s_recentColors[i].name()));
        } else {
            btn->setStyleSheet("background:#fff; border:1px solid #eee;");
        }
        btn->setProperty("recentIndex", i);
        connect(btn, &QPushButton::clicked, this, &ColorPopupWidget::onRecentColorClicked);
        recentLayout->addWidget(btn);
        m_recentColorWidgets.append(btn);
    }
    mainLayout->addLayout(recentLayout);

    // Hex输入
    QHBoxLayout* hexLayout = new QHBoxLayout;
    QLabel* hexLabel = new QLabel(this);
    QPixmap arrowPixmap("./image/hexArrow.png");
    hexLabel->setPixmap(arrowPixmap);
    hexLabel->setFixedSize(arrowPixmap.size());
    m_hexEdit = new QLineEdit(this);
    m_hexEdit->setPlaceholderText(tr("Enter color code"));
    m_hexEdit->setMaxLength(6);
    m_hexEdit->setFixedWidth(80);
    m_hexEdit->setValidator(new QRegExpValidator(QRegExp("[0-9A-Fa-f]{6}"), this));
    connect(m_hexEdit, &QLineEdit::editingFinished, this, &ColorPopupWidget::onHexEdited);
    hexLayout->addWidget(hexLabel);
    hexLayout->addWidget(m_hexEdit);
    hexLayout->addStretch();
    mainLayout->addLayout(hexLayout);

    setLayout(mainLayout);
}

void ColorPopupWidget::onColorBlockClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QColor color = btn->property("color").value<QColor>();
    m_currentColor = color;
    updateHexEdit(color);
    updateRecentColors(color);
    emit colorSelected(color);
}

void ColorPopupWidget::onRecentColorClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int idx = btn->property("recentIndex").toInt();
    if (idx < s_recentColors.size()) {
        QColor color = s_recentColors[idx];
        m_currentColor = color;
        updateHexEdit(color);
        emit colorSelected(color);
    }
}

void ColorPopupWidget::onHexEdited()
{
    QString hex = m_hexEdit->text();
    if (hex.length() == 6) {
        QColor color("#" + hex);
        if (color.isValid()) {
            m_currentColor = color;
            updateRecentColors(color);
            emit colorSelected(color);
        }
    }
}

void ColorPopupWidget::updateRecentColors(const QColor& color)
{
    if (s_recentColors.contains(color)) {
        s_recentColors.removeAll(color);
    }
    s_recentColors.prepend(color);
    if (s_recentColors.size() > MAX_RECENT_COLORS) {
        s_recentColors.removeLast();
    }

    // 用 QPushButton* 并用 background
    for (int i = 0; i < m_recentColorWidgets.size(); ++i) {
        QPushButton* btn = qobject_cast<QPushButton*>(m_recentColorWidgets[i]);
        if (i < s_recentColors.size()) {
            QPalette pal = btn->palette();
            pal.setColor(QPalette::Button, s_recentColors[i]);
            btn->setAutoFillBackground(true);
            btn->setPalette(pal);
            btn->update();
        } else {
            btn->setStyleSheet("background:#fff; border:1px solid #eee;");
        }
    }
}

void ColorPopupWidget::updateHexEdit(const QColor& color)
{
    m_hexEdit->setText(color.name().mid(1).toUpper());
}
