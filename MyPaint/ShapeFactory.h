#ifndef SHAPEFACTORY_H
#define SHAPEFACTORY_H

#include "ShapeBase.h"
#include "ShapeRect.h"
#include "ShapeEllipse.h"
#include "ShapePolygon.h"
#include "ShapeArrow.h"
#include <memory>

class ShapeFactory {
public:
    static std::unique_ptr<ShapeBase> createRect(const QRect& rect) {
        return std::make_unique<ShapeRect>(rect);
    }
    static std::unique_ptr<ShapeBase> createEllipse(const QRect& rect) {
        return std::make_unique<ShapeEllipse>(rect);
    }
    static std::unique_ptr<ShapeBase> createPolygon(const QPolygon& polygon) {
        return std::make_unique<ShapePolygon>(polygon);
    }
    static std::unique_ptr<ShapeBase> createArrow(const QLine& line) {
        return std::make_unique<ShapeArrow>(line);
    }
};

#endif // SHAPEFACTORY_H 