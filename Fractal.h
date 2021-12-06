#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <stack>
#include <memory>
#include <numeric>

struct FractalNode;

struct Fractal
{
	std::unique_ptr<FractalNode> baseNode;
	std::vector<sf::Color> colors;
	std::size_t maxDepth;
	float scale;

	Fractal(float scale = 1, std::size_t maxDepth = 5);

	void draw(sf::RenderTarget& renderTarget);
};
