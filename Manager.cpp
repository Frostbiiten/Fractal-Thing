#include "Manager.h"
#include "FractalNode.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <sstream>

namespace Manager
{
	sf::RenderWindow* windowPtr;
	sf::Vector2u resolution = sf::Vector2u(1000, 1000);

	sf::View cameraView = sf::View(sf::Vector2f(0, 0), sf::Vector2f(3.f, 3.f));
	float baseCameraMovementSpeed = 0.002f;
	float baseCameraZoomSpeed = 0.05f;
	sf::Texture screenshotTexture;
	
	float scale = 1.f;
	Fractal fractal(scale, 5);
	sf::Color bgColor;

	std::size_t deltaTimeMilliseconds;
	sf::Clock deltaTimeClock;

	sf::Text fpsText;
	sf::Font font;

	void updateCameraPosition()
	{
		//Directional movement
		sf::Vector2f inputDirection;
		inputDirection.x += sf::Keyboard::isKeyPressed(sf::Keyboard::D);
		inputDirection.x -= sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		inputDirection.y += sf::Keyboard::isKeyPressed(sf::Keyboard::S);
		inputDirection.y -= sf::Keyboard::isKeyPressed(sf::Keyboard::W);
		cameraView.move(inputDirection * baseCameraMovementSpeed * cameraView.getSize().x * (float)deltaTimeMilliseconds);

		//Zoom
		float zoomDir = 0.f;
		zoomDir += sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
		zoomDir -= sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
		cameraView.zoom(1.f + zoomDir * baseCameraMovementSpeed * deltaTimeMilliseconds);

		//Rotation
		float rotationDirection = 0.f; //Rotation current breaks culling, but this can be fixed
		rotationDirection += sf::Keyboard::isKeyPressed(sf::Keyboard::E);
		rotationDirection -= sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
		cameraView.rotate(rotationDirection);

		//View remapping for infinite* zoom
		if (cameraView.getSize().x <= 0.001f)
		{
			float scalar = 1 << fractal.colors.size();
			cameraView.setSize(cameraView.getSize() * scalar);
			sf::Vector2f cameraCenter = cameraView.getCenter();
			
			//If camera is located at the edge of the fractal...
			/*
			std::cout << std::fabs(cameraCenter.x) - cameraView.getSize().x / 2.f;
			if (std::fabs(cameraCenter.x) - cameraView.getSize().x / 2.f > 1.f)
			{
				while(false)
				{
				}
			}
			*/
			//0.0078125 is the largest negative power of 2 less than 0.001
			cameraCenter.x = fmodf(cameraCenter.x, 0.0078125f);
			cameraView.setCenter(cameraCenter * scalar);
			scale *= scalar;
		}

		windowPtr->setView(cameraView);
	}
	sf::View getCameraView()
	{
		return cameraView;
	}
	void processWindowEvents()
	{
		sf::Event event;
		sf::View v = windowPtr->getView();
		while (windowPtr->pollEvent(event))
		{
			if (event.type == event.Closed)
			{
				windowPtr->close();
			}
			else if (event.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				std::cout << "Clearing node cache\n";
				fractal.baseNode->unloadChildren();
			}
			else if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				screenshotTexture.update(*windowPtr);
				sf::Image img = screenshotTexture.copyToImage();
				std::thread t([img]()
				{
					std::stringstream fileName;
					sf::Vector2f cameraCenter = cameraView.getCenter();
					fileName << std::fixed << std::setprecision(2) << cameraView.getSize().x << '(' << cameraCenter.x << ", " << cameraCenter.y << ").png";
					img.saveToFile(fileName.str());
				});
				t.detach();
			}
		}
		windowPtr->clear(bgColor);
		deltaTimeMilliseconds = deltaTimeClock.restart().asMilliseconds();
	}
	void mainLoop()
	{
		while (windowPtr->isOpen())
		{
			processWindowEvents();
			updateCameraPosition();
			//auto v = windowPtr->getView(); Culling debug view
			//v.setSize(v.getSize() * 10.f);
			//windowPtr->setView(v);
			fractal.draw(*windowPtr);
			windowPtr->display();
			//std::cout << renderCount << '\n'; How many circles were rendered in the frame?
			renderCount = 0;
		}
	}
	void init()
	{
		windowPtr = new sf::RenderWindow(sf::VideoMode(resolution.x, resolution.y), "Fractal", 7U, sf::ContextSettings(0, 0, 8));
		screenshotTexture.create(resolution.x, resolution.y);
		bgColor = fractal.colors[fractal.colors.size() - 1];
		mainLoop();
	}
}
