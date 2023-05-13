#include "point.hpp"
#include "constants.hpp"

Node::Node(sf::Vector2f pos) : selected(false)
{
  pointSprite = sf::RectangleShape(sf::Vector2f(nodeSideLength, nodeSideLength));
  pointSprite.setFillColor(nodeColor);
  pointSprite.setPosition(pos.x - nodeSideLength/2, pos.y - nodeSideLength/2);
}

sf::Vertex Node::getVertex() const
{
  sf::Vector2f pos = pointSprite.getPosition();
  pos.x += nodeSideLength/2;
  pos.y += nodeSideLength/2;
  sf::Vertex vert(pos);
  vert.color = curveColor; 
  return vert;
}

void Node::select()
{
  selected = true;
  pointSprite.setFillColor(pickedNodeColor);
}

void Node::deselect()
{
  selected = false;
  pointSprite.setFillColor(nodeColor);
}

bool Node::isSelected()
{
  return selected;
}

sf::Vector2f Node::getPosition() const
{
  return pointSprite.getPosition();
}

void Node::setPosition(float x, float y)
{
  pointSprite.setPosition(sf::Vector2f(x - nodeSideLength/2, y - nodeSideLength/2));
}

sf::FloatRect Node::getBounds() const
{
  return pointSprite.getGlobalBounds();
}

void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  target.draw(pointSprite, states);
}
