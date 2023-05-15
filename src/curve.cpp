#include "curve.hpp"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <iostream>
#include <cmath>

Curve::Curve() : selected(false), type(CurveType::Polyline), makeCurve(makePolyline), drawStep(0.001) {}

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
  points = makeCurve(nodesList, drawStep);
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
  makeCurve = curveFunctions[type];
  this->updateCurve();
}

void Curve::moveNodes(float x, float y)
{
  for(auto& node : nodesList)
  {
    node.move(x,y); 
  }
  this->updateCurve();
}

void Curve::rotateNodes(sf::Vector2f origin, float angle)
{
  for(auto& node : nodesList)
  {
    auto pos = node.getPosition();
    auto xprim = (pos.x - origin.x) * cos(angle) - (pos.y - origin.y) * sin(angle);
    auto yprim = (pos.y - origin.y) * cos(angle) + (pos.x - origin.x) * sin(angle); 
    node.setPosition(xprim + origin.x,yprim + origin.y);
  }
  this->updateCurve();
}

CurveType Curve::getCurveType()
{
  return type;
}

void Curve::setStep(float val)
{
  drawStep = val;
  this->updateCurve();
}

float Curve::getStep()
{
  return drawStep;
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

