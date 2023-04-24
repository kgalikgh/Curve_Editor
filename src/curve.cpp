#include "curve.hpp"

Curve::Curve() : isSelected(false), points(sf::LineStrip) {}

void Curve::updatePoints()
{
  points = sf::VertexArray(sf::LineStrip);
  for(auto const& node : nodesList)
  {
    sf::Vertex vert = node.getVertex();
    points.append(vert);
  }
}

void Curve::addNode(Node node)
{
  nodesList.push_back(node);
  updatePoints();
}

void Curve::removeNode(Node node)
{
  nodesList.remove(node);
  updatePoints();
}

void Curve::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  if(nodesList.empty()) return;
  if(isSelected)
    for(auto const& node : nodesList)
      target.draw(node);
  target.draw(points);
}
