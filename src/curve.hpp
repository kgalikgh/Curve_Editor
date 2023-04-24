#pragma once
#include <SFML/Graphics.hpp>
#include "point.hpp"
#include <list>

class Curve : public sf::Drawable
{
  public:
  Curve();
  void addNode(Node);
  void removeNode(Node);
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
  bool isSelected;
  std::list<Node> nodesList;
  sf::VertexArray points;
  void updatePoints();
};

class LagrangeInterpolationCurve : public Curve {};
