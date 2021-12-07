#pragma once
#include <SFML/Graphics.hpp>
#include "Fractal.h"

namespace Manager
{
	inline std::size_t renderCount = 0;
	void init();
	void updateCameraPosition();
	sf::View getCameraView();
}