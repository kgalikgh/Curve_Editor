#ifndef CURVE_FUNCTIONS_HPP
#define CURVE_FUNCTIONS_HPP
#include <SFML/Graphics.hpp>
#include "point.hpp"

sf::VertexArray makeLagrangeInterpolation(const std::vector<Node>&, float step);
sf::VertexArray makePolyline(const std::vector<Node>&, float step);
sf::VertexArray makeBezier(const std::vector<Node>&, float step);
#endif
