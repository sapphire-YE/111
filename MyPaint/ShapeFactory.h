#ifndef SHAPEFACTORY_H
#define SHAPEFACTORY_H

#include "ShapeArrow.h"
#include "ShapeBase.h"
#include "ShapeEllipse.h"
#include "ShapeRect.h"
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
};

#endif // SHAPEFACTORY_H