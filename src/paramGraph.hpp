#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

// Create dataset for parametric function
sf::VertexArray makeParamGraph(float a, float b, float step, std::function<float(float)> fun_x, std::function<float(float)> fun_y);

// Translate dataset by a vector
sf::VertexArray translateByVector(sf::Vector2f vec, sf::VertexArray arr);
