#include "point.hpp"

const sf::Color nodeColor = sf::Color(102,102,102);
const sf::Color vertexColor = sf::Color(30,30,30);
const sf::Color pickedNodeColor = sf::Color(200, 100, 100);
static const int nodeSideLength = 8;

Node::Node(sf::Vector2f pos) : isSelected(false)
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
  sf::Vertex vert (pos);
  vert.color = vertexColor; 
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
