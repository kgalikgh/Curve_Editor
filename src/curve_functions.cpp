#include "curve_functions.hpp"
#include "point.hpp"
#include "constants.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <cstdint>

sf::VertexArray makePolyline(const std::vector<Node>& nodesList, float step)
{
  (void)step;
  auto points = sf::VertexArray(sf::LineStrip);
  for(auto& node : nodesList)
    points.append(node.getVertex()); 
  return points;
}

//Barycentric lagrange
sf::VertexArray makeLagrangeInterpolation(const std::vector<Node>& nodesList, float step)
{
  auto points = sf::VertexArray(sf::LineStrip);
  int n = nodesList.size() - 1;
  double chebNodes[n+1];

  //Chebyshev nodes
  for(int i = 1; i <= n+1; i++)
  {
    chebNodes[i-1] = cos((2.0*i-1.0)/(2.0*(n+1)) * M_PI);
  }  
  double a = chebNodes[n];
  double b = chebNodes[0];

  //Werner algorithm
  double* helpers = new double[(n+1) * (n+1)]; 
  helpers[0] = 1;
  for(int k = 1; k <= n; k++) helpers[k] = 0;
  for(int i = 1; i <= n; i++)
  {
    for(int k = 0; k <= i-1; k++)
    {
      helpers[i* (n+1) + k] = helpers[(i-1) * (n+1) + k]/(chebNodes[k] - chebNodes[i]);
      helpers[(k+1) * (n+1) + i] = helpers[k *(n+1) + i]-helpers[i * (n+1) + k];
    }
  } 

  for(double t = a; t <= b; t+= step)
  {
    double x = 0.0;
    double y = 0.0;
    bool skip = false;
    for(int i = 0; i <= n; i++)
    {
      if(t == chebNodes[i])
      {
        auto pos = nodesList[i].getPosition();
        x = pos.x;
        y = pos.y;    
        skip = true;
        break; 
      }
    }
    if(!skip)
    {
      double divisor = 0.0;
      for(int i = 0; i <= n; i++)
      {
        divisor+= helpers[n*(n+1) + i]/(t - chebNodes[i]);
      }
      for(int i = 0; i <= n; i++)
      {
        auto pos = nodesList[i].getPosition();
        x += pos.x * helpers[n * (n+1) + i]/(t - chebNodes[i]);
        y += pos.y * helpers[n * (n+1) + i]/(t - chebNodes[i]);
      }
      x/=divisor;
      y/=divisor;
    }
    auto vert = sf::Vertex(sf::Vector2f(x,y));
    vert.color = curveColor; 
    points.append(vert);
  } 
  delete[] helpers;
  return points;
}


// Bezier
sf::VertexArray makeBezier(const std::vector<Node>& nodesList, float step)
{
  auto points = sf::VertexArray(sf::LineStrip);
  int n = nodesList.size() - 1;
  
  double* w_x = new double[(n+1) * (n+1)];
  double* w_y = new double[(n+1) * (n+1)];
  /*
    b[0][i] = nodesList[i]
    b[1][i] = b[0][i] * t + b[0][i+1] * (1-t)

  */
  for(float t = 0.0; t <= 1.0; t+= step)
  {
    double x = 0;
    double y = 0;
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
    auto vert = sf::Vertex(sf::Vector2f(w_x[n*(n+1)],w_y[n*(n+1)]));
    vert.color = curveColor; 
    points.append(vert);
  }
  delete[] w_x;
  delete[] w_y;
  return points;
}
