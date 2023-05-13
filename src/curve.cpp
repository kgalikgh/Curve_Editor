#include "curve.hpp"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <iostream>

sf::VertexArray makePolyline(const std::vector<Node>& nodesList)
{
  auto points = sf::VertexArray(sf::LineStrip);
  for(auto& node : nodesList)
    points.append(node.getVertex()); 
  return points;
}

sf::VertexArray makeLagrangeInterpolation(const std::vector<Node>& nodesList)
{
  auto points = sf::VertexArray(sf::LineStrip);
  for(auto& node : nodesList)
  {
    auto v = node.getVertex();
    v.color = sf::Color::Red;
    points.append(v); 
  }
  return points;
}

Curve::Curve() : selected(false), type(CurveType::Polyline), makeCurve(makePolyline) {}

void Curve::addNode(Node node)
{
  nodesList.push_back(node);
  updateCurve();
}

void Curve::removeNode(int index)
{
  nodesList.erase(nodesList.begin() + index);
  updateCurve();
}

void Curve::select()
{
  selected = true;
}

void Curve::deselect()
{
  selected = false;
}

bool Curve::isSelected()
{
  return selected;
}

void Curve::updateCurve()
{
  points = makeCurve(nodesList);
}

Node* Curve::findClickedNode(tgui::Vector2f pos)
{
  Node* ret = nullptr;
  for(auto& node : nodesList)
  {
    auto bounds = node.getBounds();
    if(bounds.contains(pos))
    {
      ret = &node;
      break;
    }
  }
  return ret;
}

void Curve::removeClickedNode(tgui::Vector2f pos)
{
  for(int i = 0; i < nodesList.size(); ++i)
  {
    auto point = nodesList[i];
    auto bounds = point.getBounds();
    if(bounds.contains(pos))
    {
      this->removeNode(i);
      break;
    }
  }
}

void Curve::changeCurveType(CurveType newType)
{
  type = newType;
  switch(type)
  {
    case CurveType::Polyline:
    makeCurve = makePolyline; 
    break; 
    case CurveType::LagrangeInterpolation:
    makeCurve = makeLagrangeInterpolation; 
    break; 
  } 
  this->updateCurve();
}

void Curve::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  if(nodesList.empty()) return;
  if(selected)
  {
    for(auto const& node : nodesList)
    {
      target.draw(node);
    }
  }
  target.draw(points);
}

