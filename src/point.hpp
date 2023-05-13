#pragma once
#include <SFML/Graphics.hpp>

//TODO
//Copy and Move constructors
class Node : public sf::Drawable
{
  public:
  Node(sf::Vector2f);
  void select();
  void deselect();
  bool isSelected();

  sf::Vector2f getPosition() const;
  sf::FloatRect getBounds() const;
  void setPosition(float x, float y);

  sf::Vertex getVertex() const;
  
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  private:
  sf::RectangleShape pointSprite;
  bool selected;
};
