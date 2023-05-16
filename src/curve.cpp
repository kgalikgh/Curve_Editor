#include "curve.hpp"
#include "constants.hpp"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <iostream>
#include <cmath>

Curve::Curve() : selected(false), type(CurveType::Polyline), makeCurve(makePolyline), thickness(thickMin) {}

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
  points = makeCurve(nodesList, thickness);
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

void Curve::setThickness(float val)
{
  thickness = val;
  this->updateCurve();
}

float Curve::getThickness()
{
  return thickness;
}

void Curve::setStepMult(int val)
{
  stepMult = val;
  this->updateCurve();
}

int Curve::getStepMult()
{
  return stepMult;
}

void Curve::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  if(nodesList.empty()) return;
  target.draw(points);
  if(selected)
  {
    for(auto const& node : nodesList)
    {
      target.draw(node);
    }
  }
}

void Curve::elevate()
{
  if(type != CurveType::Bezier) return;
  std::vector<Node> newNodesList;
  int n = nodesList.size() - 1;
  newNodesList.push_back(nodesList[0]);
  for(int i = 1; i < n+1; i++)
  {
    sf::Vector2f pos1 = nodesList[i-1].getPosition();
    sf::Vector2f pos2 = nodesList[i].getPosition();
    sf::Vector2f res = ((float)i * pos1 + (float)(n+1-i) * pos2)/(float)(n+1);
    Node n(res);
    newNodesList.push_back(n);
  }
  newNodesList.push_back(nodesList[n]);
  nodesList = newNodesList;
  this->updateCurve();
}

void Curve::divide(float t, std::vector<Node>& c1, std::vector<Node>& c2)
{
  int n = nodesList.size() - 1;
  std::cout<<"cool\n";
  
  double* w_x = new double[(n+1) * (n+1)];
  double* w_y = new double[(n+1) * (n+1)];
  /*
    b[0][i] = nodesList[i]
    b[1][i] = b[0][i] * t + b[0][i+1] * (1-t)
  */
  for(int i = 0; i <= n; i++) 
  {
    auto pos = nodesList[i].getPosition();
    w_x[i] = pos.x;
    w_y[i] = pos.y;
  }
  
  for(int i = 1 ; i <= n; ++i)
  {
    for(int k = 0; k <= n - i; k++)
    {
      w_x[i * (n+1) + k] = t * w_x[(i-1)*(n+1) + k] + (1.0-t) *  w_x[(i-1)*(n+1) + k+1];
      w_y[i * (n+1) + k] = t * w_y[(i-1)*(n+1) + k] + (1.0-t) *  w_y[(i-1)*(n+1) + k+1];
    } 
  }

  for(int i = 0; i <= n; i++)
  {
    Node n1(sf::Vector2f(w_x[i * (n+1)], w_y[i * (n+1)]));
    Node n2(sf::Vector2f(w_x[(n-i) * (n+1) + i], w_y[(n-i) * (n+1) + i]));
    c1.push_back(n1);
    c2.push_back(n2);
  }
  delete[] w_x;
  delete[] w_y;
}
