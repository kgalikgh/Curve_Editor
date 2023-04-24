#pragma once
#include <SFML/Graphics.hpp>

class Node : public sf::Drawable
{
  public:
  Node(sf::Vector2f);
  sf::Vertex getVertex() const;
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
  friend bool operator==(const Node&, const Node&);
  sf::RectangleShape pointSprite;
  bool isSelected;
};
