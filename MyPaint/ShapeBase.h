#pragma once
#include <QJsonObject>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <memory>
#include <vector>
#include <QJsonObject>
#include <memory>
#include <QColor>

class ShapeArrow; // 前置声明

// 1. 先在外部声明 Handle
struct ShapeHandle
{
  QRect rect;
  enum Type
  {
    Scale,
    Arrow,
    ArrowAnchor,
    Rotate // 旋转锚点类型
  } type;
  int direction;
};

class ShapeBase
{
public:
  virtual ~ShapeBase() {}

  // 纯虚函数，子类必须实现
  virtual void paintShape(QPainter *painter) = 0; // 只绘制图形本身
  virtual bool contains(const QPoint &pt) const = 0;
  virtual void moveBy(const QPoint &delta) = 0;
  virtual void resize(const QRect &newRect) = 0;
  virtual QRect boundingRect() const = 0;
  virtual void rotate(double angle) = 0; // 旋转图形

  // 复制和矩形操作相关方法
  virtual std::unique_ptr<ShapeBase> clone() const = 0;     // 克隆当前图形
  virtual QRect getRect() const { return boundingRect(); }  // 获取矩形区域
  virtual void setRect(const QRect &rect) { resize(rect); } // 设置矩形区域

  // 统一用 ShapeHandle
  using Handle = ShapeHandle;

  // 在基类中实现的共同功能
  void paint(QPainter *painter, bool selected = false);

  // 默认实现八个缩放锚点，子类可以重写
  virtual bool needPlusHandles() const { return true; }
  virtual std::vector<Handle> getHandles() const;

  // 获取箭头锚点，默认返回空
  virtual std::vector<Handle> getArrowAnchors() const { return {}; }

  // 将加号锚点索引映射到对应的ArrowAnchor锚点索引
  virtual int mapArrowHandleToAnchor(int arrowHandleIndex) const {
    // 默认实现：加号锚点的direction值从9开始，对应ArrowAnchor从0开始
    return arrowHandleIndex - 9;
  }

  // 旋转相关方法
  virtual double getRotation() const { return m_rotation; }      // 获取当前旋转角度
  virtual void setRotation(double angle) { m_rotation = angle; } // 设置旋转角度

  // 处理锚点交互
  bool handleAnchorInteraction(const QPoint &mousePos,
                               const QPoint &lastMousePos);
  bool isHandleSelected() const { return m_selectedHandleIndex != -1; }
  void clearHandleSelection() { m_selectedHandleIndex = -1; }
  int getSelectedHandleIndex() const { return m_selectedHandleIndex; }
  void setSelectedHandleIndex(int index) { m_selectedHandleIndex = index; }

  // 文本相关方法
  virtual bool isTextEditable() const
  {
    return true;
  } // 默认所有图形都可编辑文本，除了箭头
  virtual void setText(const QString &text) { m_text = text; }
  virtual QString getText() const { return m_text; }
  virtual bool isEditing() const { return m_isEditing; }
  virtual void setEditing(bool editing) { m_isEditing = editing; }

  // 序列化相关方法
  virtual QJsonObject toJson() const
  {
    QJsonObject obj;
    QRect rect = boundingRect();
    obj["x"] = rect.x();
    obj["y"] = rect.y();
    obj["width"] = rect.width();
    obj["height"] = rect.height();
    obj["text"] = m_text;
    obj["lineColor"] = m_lineColor.name();
    obj["lineWidth"] = m_lineWidth;
    obj["lineType"] = static_cast<int>(m_lineType); // 保存线条类型
    obj["fillColor"] = m_fillColor.name();
    obj["opacity"] = m_opacity;

    // 保存字体和文本相关属性
    if (!m_text.isEmpty())
    {
      obj["textColor"] = m_textColor.name();
      obj["fontFamily"] = m_font.family();
      obj["fontSize"] = m_font.pointSize();
      obj["textAlignment"] = m_textAlignment;
      obj["fontBold"] = m_font.bold();
      obj["fontItalic"] = m_font.italic();
      obj["fontUnderline"] = m_font.underline();
      obj["fontStrikeOut"] = m_font.strikeOut();
    }

    return obj;
  }

  virtual void fromJson(const QJsonObject &obj)
  {
    QRect rect(
        obj["x"].toInt(),
        obj["y"].toInt(),
        obj["width"].toInt(),
        obj["height"].toInt());
    resize(rect);
    if (obj.contains("text"))
    {
      m_text = obj["text"].toString();
    }
    if (obj.contains("lineColor"))
    {
      m_lineColor = QColor(obj["lineColor"].toString());
    }
    if (obj.contains("lineWidth"))
    {
      m_lineWidth = obj["lineWidth"].toInt();
    }
    if (obj.contains("lineType"))
    {
      m_lineType = static_cast<LineType>(obj["lineType"].toInt());
    }
    if (obj.contains("fillColor"))
    {
      m_fillColor = QColor(obj["fillColor"].toString());
    }

    if (obj.contains("opacity"))
    {
      m_opacity = obj["opacity"].toDouble();
    }

    // 加载字体和文本相关属性
    if (obj.contains("textColor"))
    {
      m_textColor = QColor(obj["textColor"].toString());
    }

    QFont font = m_font;
    bool fontChanged = false;

    if (obj.contains("fontFamily"))
    {
      font.setFamily(obj["fontFamily"].toString());
      fontChanged = true;
    }

    if (obj.contains("fontSize"))
    {
      font.setPointSize(obj["fontSize"].toInt());
      fontChanged = true;
    }

    if (obj.contains("fontBold"))
    {
      font.setBold(obj["fontBold"].toBool());
      fontChanged = true;
    }

    if (obj.contains("fontItalic"))
    {
      font.setItalic(obj["fontItalic"].toBool());
      fontChanged = true;
    }

    if (obj.contains("fontUnderline"))
    {
      font.setUnderline(obj["fontUnderline"].toBool());
      fontChanged = true;
    }

    if (obj.contains("fontStrikeOut"))
    {
      font.setStrikeOut(obj["fontStrikeOut"].toBool());
      fontChanged = true;
    }

    if (fontChanged)
    {
      m_font = font;
    }

    if (obj.contains("textAlignment"))
    {
      m_textAlignment = obj["textAlignment"].toInt();
    }
  }

  // 线条类型枚举
  enum LineType
  {
    SolidLine = 0, // 实线
    DashLine = 1,  // 虚线
    DotLine = 2    // 点线
  };

  // 线条样式相关方法
  virtual void setLineColor(const QColor &color) { m_lineColor = color; }
  virtual QColor getLineColor() const { return m_lineColor; }
  virtual void setLineWidth(int width) { m_lineWidth = width; }
  virtual int getLineWidth() const { return m_lineWidth; }
  virtual void setLineType(LineType type) { m_lineType = type; }
  virtual LineType getLineType() const { return m_lineType; }

  // 填充颜色相关方法
  virtual void setFillColor(const QColor &color) { m_fillColor = color; }
  virtual QColor getFillColor() const { return m_fillColor; }

  // 不透明度相关方法
  virtual void setOpacity(double opacity) { m_opacity = opacity; }
  virtual double getOpacity() const { return m_opacity; }

  // 文本样式相关方法
  virtual void setTextColor(const QColor &color) { m_textColor = color; }
  virtual QColor getTextColor() const { return m_textColor; }
  virtual void setFont(const QFont &font) { m_font = font; }
  virtual QFont getFont() const { return m_font; }
  virtual void setFontSize(int size)
  {
    QFont font = m_font;
    font.setPointSize(size);
    m_font = font;
  }
  virtual int getFontSize() const { return m_font.pointSize(); }
  virtual void setFontFamily(const QString &family)
  {
    QFont font = m_font;
    font.setFamily(family);
    m_font = font;
  }
  virtual QString getFontFamily() const { return m_font.family(); }
  virtual void setTextAlignment(int alignment) { m_textAlignment = alignment; }
  virtual int getTextAlignment() const { return m_textAlignment; }

  // 字体样式相关方法
  virtual void setFontBold(bool bold)
  {
    QFont font = m_font;
    font.setBold(bold);
    m_font = font;
  }
  virtual bool isFontBold() const { return m_font.bold(); }

  virtual void setFontItalic(bool italic)
  {
    QFont font = m_font;
    font.setItalic(italic);
    m_font = font;
  }
  virtual bool isFontItalic() const { return m_font.italic(); }

  virtual void setFontUnderline(bool underline)
  {
    QFont font = m_font;
    font.setUnderline(underline);
    m_font = font;
  }
  virtual bool isFontUnderline() const { return m_font.underline(); }

  virtual void setFontStrikeOut(bool strikeOut)
  {
    QFont font = m_font;
    font.setStrikeOut(strikeOut);
    m_font = font;
  }
  virtual bool isFontStrikeOut() const { return m_font.strikeOut(); }

protected:
  // 计算新的矩形区域
  QRect calculateNewRect(const QPoint &mousePos,
                         const QPoint &lastMousePos) const;

  // 文本相关属性
  QString m_text;
  bool m_isEditing = false;
  double m_rotation = 0.0;               // 旋转角度（弧度）
  QColor m_lineColor = Qt::black;        // 线条颜色
  QColor m_fillColor = Qt::white;        // 填充颜色
  QColor m_textColor = Qt::black;        // 文本颜色
  QFont m_font;                          // 字体
  int m_textAlignment = Qt::AlignCenter; // 文本对齐方式
  int m_lineWidth = 1;                   // 线条粗细
  LineType m_lineType = SolidLine;       // 线条类型，默认为实线
  double m_opacity = 1.0;                // 不透明度（0.0-1.0）

private:
  int m_selectedHandleIndex = -1; // 当前选中的锚点索引
};
