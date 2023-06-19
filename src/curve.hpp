#ifndef CURVE_HPP
#define CURVE_HPP

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <map>
#include "point.hpp"

enum CurveType {Polyline, LagrangeInterpolation, Bezier};

class Curve : public sf::Drawable
{
  public:
  void addNode(Node);
  void removeNode(int);
  void select();
  void deselect();
  bool isSelected() const;
  Node* findClickedNode(tgui::Vector2f);
  void removeClickedNode(tgui::Vector2f);
  CurveType getCurveType();
  std::vector<Node> getNodes() const;
  void moveNodes(float, float);
  void rotateNodes(sf::Vector2f, float);
  void setThickness(float);
  float getThickness();

  virtual void updateCurve() { std::cout<<"I called base method\n";} 
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  protected:
  explicit Curve(CurveType);
  Curve(CurveType, const Curve&);

  bool selected;
  std::vector<Node> nodesList;
  sf::VertexArray points;
  CurveType type;
  float thickness;
};

class BezierCurve : public Curve
{
  public:
  BezierCurve() : Curve(CurveType::Bezier) { std::cout<<"Created Bezier\n";}
  explicit BezierCurve(const Curve& c) : Curve(CurveType::Bezier, c) {std::cout<<"Created Bezier\n";}
  
  void elevate();
  void deelevate();
  void updateCurve() override;
  void divide(float, std::vector<Node>&, std::vector<Node>&);
  sf::Vector2f getBezierVal(float);
};

class PolylineCurve : public Curve
{
  public:
  PolylineCurve() : Curve(CurveType::Polyline) {std::cout<<"Created Polyline\n";}
  explicit PolylineCurve(const Curve& c) : Curve(CurveType::Polyline, c) {std::cout<<"Created Polyline\n";}
  void updateCurve() override;
};

#endif
