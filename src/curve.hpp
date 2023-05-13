#ifndef CURVE_HPP
#define CURVE_HPP

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include "point.hpp"

enum CurveType {Polyline, LagrangeInterpolation};

//TODO
//Copy and Move constructors
class Curve : public sf::Drawable
{
  public:
  Curve();
  void addNode(Node);
  void removeNode(int);
  void select();
  void deselect();
  bool isSelected();
  Node* findClickedNode(tgui::Vector2f);
  void removeClickedNode(tgui::Vector2f);
  void changeCurveType(CurveType);

  //void moveAllPointsByOffset(sf::vector2f);
  void updateCurve();
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  private:
  bool selected;
  std::vector<Node> nodesList;
  sf::VertexArray points;
  CurveType type;
  //TODO
  //Implement variable precision
  float drawStep;
  std::function<sf::VertexArray(const std::vector<Node>&)> makeCurve;
};

#endif
