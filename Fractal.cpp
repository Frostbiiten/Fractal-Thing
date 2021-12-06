#include "Fractal.h"
#include "FractalNode.h"
#include "Manager.h"
#include <iostream>
#include <fstream>
#include <sstream>

Fractal::Fractal(float scale, std::size_t maxDepth)
{
	this->scale = scale;
	this->maxDepth = maxDepth;
	
	std::string paletteFileName;
	std::ifstream paletteFile("paletteName.txt");
	std::getline(paletteFile, paletteFileName);

	std::ifstream paletteStream;
	paletteStream.open("Palettes/" + paletteFileName);
	if (paletteStream.is_open())
	{
		std::string line;
		while (std::getline(paletteStream, line))
		{
			std::stringstream colorStream(line);
			std::string colorString;
			sf::Color color = sf::Color::White;
			std::size_t index = 0;
			while (std::getline(colorStream, colorString, ','))
			{
				if (index == 0) color.r = std::stoi(colorString);
				else if (index == 1) color.g = std::stoi(colorString);
				else if (index == 2) color.b = std::stoi(colorString);
				else color.a = std::stoi(colorString);
				index++;
			}
			colors.push_back(color);
		}
	}
	else
	{
		std::cout << "Invalid palette\n";
		paletteStream.close();
		colors =
		{
			sf::Color(sf::Color::White),
			sf::Color(sf::Color(255, 122, 255)),
			sf::Color(sf::Color::Blue)
		};
	}

	baseNode = std::make_unique<FractalNode>(this, 0, scale, sf::Vector2f(), 0, maxDepth);
}

void Fractal::draw(sf::RenderTarget& renderTarget)
{
	FractalNode::updateCameraProperties();
	baseNode->draw(renderTarget);
}
