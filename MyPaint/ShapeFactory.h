#ifndef SHAPEFACTORY_H
#define SHAPEFACTORY_H

#include "ShapeArrow.h"
#include "ShapeBase.h"
#include "ShapeEllipse.h"
#include "ShapeRect.h"
#include "ShapePentagon.h"
#include "ShapeTriangle.h"
#include "ShapeDiamond.h"
#include "ShapeRoundedRect.h"
#include <memory>

class ShapeFactory
{
public:
  static std::unique_ptr<ShapeBase> createRect(const QRect &rect)
  {
    return std::unique_ptr<ShapeBase>(new ShapeRect(rect));
  }
  static std::unique_ptr<ShapeBase> createEllipse(const QRect &rect)
  {
    return std::unique_ptr<ShapeBase>(new ShapeEllipse(rect));
  }
  static std::unique_ptr<ShapeBase> createArrow(const QLine &line)
  {
    return std::unique_ptr<ShapeBase>(new ShapeArrow(line));
  }
  static std::unique_ptr<ShapeBase> createPentagon(const QRect &rect)
  {
    return std::unique_ptr<ShapeBase>(new ShapePentagon(rect));
  }
  static std::unique_ptr<ShapeBase> createTriangle(const QRect &rect)
  {
    return std::unique_ptr<ShapeBase>(new ShapeTriangle(rect));
  }
  static std::unique_ptr<ShapeBase> createDiamond(const QRect &rect)
  {
    return std::unique_ptr<ShapeBase>(new ShapeDiamond(rect));
  }
  static std::unique_ptr<ShapeBase> createRoundedRect(const QRect &rect)
  {
    return std::unique_ptr<ShapeBase>(new ShapeRoundedRect(rect));
  }
};

#endif // SHAPEFACTORY_H
