#pragma once
#include <SFML/Graphics.hpp>
#include "Fractal.h"

namespace Manager
{
	void init();
	void updateCameraPosition();
	sf::View getCameraView();
}