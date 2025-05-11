#pragma once
#include <QColor>
#include <QVector>
#include <QWidget>

class QLineEdit;

class ColorPopupWidget : public QWidget {
  Q_OBJECT
public:
  explicit ColorPopupWidget(QWidget *parent = nullptr);

signals:
  void colorSelected(const QColor &color);

private slots:
  void onColorBlockClicked();
  void onRecentColorClicked();
  void onHexEdited();

private:
  void createColorBlocks();
  void updateRecentColors(const QColor &color);
  void updateHexEdit(const QColor &color);

  QVector<QColor> m_systemColors;
  QVector<QWidget *> m_recentColorWidgets;
  QLineEdit *m_hexEdit;
  QColor m_currentColor;

  // 添加静态成员变量来存储历史颜色
  static QVector<QColor> s_recentColors;
  static const int MAX_RECENT_COLORS = 8;
};
