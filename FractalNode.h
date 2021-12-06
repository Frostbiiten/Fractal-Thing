#pragma once
#include <SFML/Graphics.hpp>
#include "Fractal.h"

struct FractalNode
{
	Fractal* fractal;
	sf::CircleShape shape;
	sf::Vector2f center;
	float radius;
	std::size_t depth;
	std::size_t halfMultiplier;

	std::vector<FractalNode> childrenNodes;

	FractalNode();
	FractalNode(Fractal* fractal, std::size_t halfMult, float radius, sf::Vector2f center, std::size_t depth, const std::size_t& maxDepth);

	void loadChildren();
	void unloadChildren();
	void draw(sf::RenderTarget& renderTarget);

	static void updateCameraProperties();
};
