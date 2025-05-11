#ifndef SHAPEPOLYGON_H
#define SHAPEPOLYGON_H

#include "ShapeBase.h"
#include <QPolygon>

class ShapePolygon : public ShapeBase {
public:
    ShapePolygon(const QPolygon& polygon);
    void paintShape(QPainter* painter) override;
    bool contains(const QPoint& pt) const override;
    void moveBy(const QPoint& delta) override;
    void resize(const QRect& newRect) override;
    QRect boundingRect() const override;
private:
    QPolygon m_polygon;
};

#endif // SHAPEPOLYGON_H 