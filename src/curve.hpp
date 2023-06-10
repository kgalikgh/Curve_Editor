#ifndef CURVE_HPP
#define CURVE_HPP

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <map>
#include "point.hpp"
#include "curve_functions.hpp"

enum CurveType {Polyline, LagrangeInterpolation, Bezier};
static std::map<CurveType, std::function<sf::VertexArray(const std::vector<Node>&, float)>> curveFunctions{
  {Polyline, makePolyline},
  {LagrangeInterpolation, makeLagrangeInterpolation},
  {Bezier, makeBezier}
};

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
  CurveType getCurveType();
  void moveNodes(float, float);
  void rotateNodes(sf::Vector2f, float);
  void setThickness(float);
  float getThickness();
  void setStepMult(int);
  int getStepMult();
  sf::Vector2f getBezierVal(float);

  void updateCurve();
  void elevate();
  void deelevate();
  void divide(float, std::vector<Node>&, std::vector<Node>&);
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  private:
  bool selected;
  std::vector<Node> nodesList;
  sf::VertexArray points;
  CurveType type;
  //TODO
  //Implement variable precision
  float thickness;
  int stepMult;
  std::function<sf::VertexArray(const std::vector<Node>&, float)> makeCurve;
};

#endif
