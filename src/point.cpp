#include "point.hpp"

static const sf::Color nonSelectColor = sf::Color(217,217,217);
static const sf::Color selectColor = sf::Color(102,102,102);
static const int nodeSideLength = 8;

Node::Node(sf::Vector2f pos) : isSelected(false)
{
  pointSprite = sf::RectangleShape(sf::Vector2f(nodeSideLength, nodeSideLength));
  pointSprite.setFillColor(nonSelectColor);
  pointSprite.setPosition(pos.x - nodeSideLength/2, pos.y - nodeSideLength/2);
}

sf::Vertex Node::getVertex() const
{
  sf::Vector2f pos = pointSprite.getPosition();
  pos.x += nodeSideLength/2;
  pos.y += nodeSideLength/2;
  sf::Vertex vert (pos);
  return vert;
}

void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  target.draw(pointSprite, states);
}

bool operator==(const Node& n1, const Node& n2)
{
   return n1.pointSprite.getPosition() == n2.pointSprite.getPosition(); 
}
