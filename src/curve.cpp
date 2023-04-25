#include "curve.hpp"
#include <cmath>
#include <iterator>
#include <numbers>
#include <iostream>


float getChebyshevNode(float a, float b, int i, int n)
{
  return 0.5 * (a + b) + 0.5 * (b - a) * cos((2.0*i - 1)/n*(3.14159/2));
}

Curve::Curve() : isSelected(false), points(sf::LineStrip) {}

void Curve::addNode(Node node)
{
  nodesList.push_back(node);
  updatePoints();
}

void Curve::removeNode(int index)
{
  nodesList.erase(nodesList.begin() + index);
  updatePoints();
}

void Curve::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  if(nodesList.empty()) return;
  if(isSelected)
    for(auto const& node : nodesList)
      target.draw(node);
  target.draw(points);
}

void Curve::updatePoints()
{
  std::cout<<"Points drawn" <<std::endl;
  int n = nodesList.size();
  float xk[100];

  // Finding n Chebyshev nodes
  for(int i = 1; i <= n; i++)
  {
    xk[i] = getChebyshevNode(0, 20, i, n); 
    std::cout<<"Chebyshev node:"<<xk[i]<<std::endl;
  }
  
  // Calculating w_k
  float w[100][100];
  
  w[0][0] = 1.0;
  for(int i = 1; i <= n; i++)
  {
    w[i][i] = 1;
    for(int k = 0; k < n; k++)
    {
      w[k][i] = w[k][i-1]/(xk[k] - xk[i]); 
      w[i][k+1] = w[i][k] - w[k][i];
    }
  } 
  
  sf::Vector2f positions[100];
  for(int i = 0; i < n; i++)
  {
    positions[i] = nodesList[i].pointSprite.getPosition();   
  }

  points = sf::VertexArray(sf::LineStrip);
  for(float t = 0; t <= 20; t+= 0.0001)
  {
    float x_numerator = 0.0;
    float y_numerator = 0.0;
    float denominator = 0.0;
    for(int i = 0; i <= n; i++)
    {
      float multiplier = (w[i][n]/(t - xk[i]));  
      x_numerator += positions[i].x * multiplier;
      y_numerator += positions[i].y * multiplier;
      denominator += multiplier;
    }   
    points.append(sf::Vertex(sf::Vector2f(x_numerator/denominator, y_numerator/denominator)));
  }
}

