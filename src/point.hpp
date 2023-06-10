#pragma once
#include <SFML/Graphics.hpp>

static const int nodeSideLength = 8;
static const sf::Color defaultNodeColor = sf::Color(102,102,102);
static const sf::Color selectedNodeColor = sf::Color(200, 100, 100);

class Node : public sf::Drawable
{
  public:
  Node();
  Node(sf::Vector2f);
  Node(const Node&);
  void select();
  void deselect();
  bool isSelected();

  sf::FloatRect getBounds() const;
  sf::Vector2f getPosition() const;
  void setPosition(float x, float y);
  void move(float x, float y);

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  private:
  sf::RectangleShape pointSprite;
  bool selected;
};
