#include "paramGraph.hpp"

sf::VertexArray makeParamGraph(float a, float b, float step, std::function<float(float)> fun_x, std::function<float(float)> fun_y)
{
	sf::VertexArray lines(sf::LineStrip);
	for(float i = a; i <= b; i+=step)
	{
		lines.append(sf::Vertex(sf::Vector2f(fun_x(i), fun_y(i))));
	}
	return lines;
}

sf::VertexArray translateByVector(sf::Vector2f vec, sf::VertexArray arr)
{
	std::size_t arr_size = arr.getVertexCount();
	for(std::size_t i = 0; i < arr_size; i++)
	{
		arr[i].position += vec;
	}
	return arr;
}


