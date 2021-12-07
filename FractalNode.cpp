#include "FractalNode.h"
#include "Fractal.h"
#include "Manager.h"

constexpr float renderZoomThreshold = 0.001f;
constexpr float loadThreshold = 0.004f;
constexpr float unloadThreshold = 0.002f;
//constexpr float minRes = 30;
//constexpr float maxRes = 120;

sf::View cameraView;
sf::Vector2f cameraViewSize;
sf::Vector2f cameraViewCenter;
float cameraBoundsLeft;
float cameraBoundsRight;
float cameraBoundsTop;
float cameraBoundsBottom;
sf::Vector2f scl;

FractalNode::FractalNode()
{
	depth = 0;
	center = sf::Vector2f();
	halfMultiplier = 0;
	radius = 1;
}

FractalNode::FractalNode(Fractal* fractal, std::size_t halfMult, float radius, sf::Vector2f center, std::size_t depth, const std::size_t& maxDepth)
{
	this->radius = radius;
	this->center = center;
	this->depth = depth;
	this->fractal = fractal;
	halfMultiplier = halfMult;

	shape = sf::CircleShape(radius, 25);
	shape.setOrigin(sf::Vector2f(radius, radius));
	shape.setPosition(center);
	shape.setFillColor(fractal->colors[depth % fractal->colors.size()]);

	if (depth < maxDepth)
	{
		childrenNodes.reserve(4);

		float insideNodeRadius = radius / 2.f;
		sf::Vector2f insideNodeOffset = sf::Vector2f(radius / 2.f, 0.f);
		childrenNodes.push_back(FractalNode(fractal, halfMultiplier, insideNodeRadius, center + insideNodeOffset, depth + 1, maxDepth)); // Node 1 (right)
		childrenNodes.push_back(FractalNode(fractal, halfMultiplier, insideNodeRadius, center - insideNodeOffset, depth + 1, maxDepth)); // Node 2 (left)

		float outsideNodeRadius = insideNodeRadius / 2.f;
		sf::Vector2f outsideNodeOffset = sf::Vector2f(0.f, radius + outsideNodeRadius);
		childrenNodes.push_back(FractalNode(fractal, halfMultiplier + 1, outsideNodeRadius, center + outsideNodeOffset, depth + 1, maxDepth)); //Node 3 (top)
		childrenNodes.push_back(FractalNode(fractal, halfMultiplier + 1, outsideNodeRadius, center - outsideNodeOffset, depth + 1, maxDepth)); //Node 4 (bottom)
	}
}

void FractalNode::loadChildren()
{
	childrenNodes.reserve(4);

	//Half size
	float insideNodeRadius = radius / 2.f;

	//Inside offset
	sf::Vector2f insideNodeOffset = sf::Vector2f(radius / 2.f, 0.f);
	childrenNodes.push_back(FractalNode(fractal, halfMultiplier, insideNodeRadius, center + insideNodeOffset, depth + 1, 0)); // Node 1 (right)
	childrenNodes.push_back(FractalNode(fractal, halfMultiplier, insideNodeRadius, center - insideNodeOffset, depth + 1, 0)); // Node 2 (left)

	float outsideNodeRadius = insideNodeRadius / 2.f;
	sf::Vector2f outsideNodeOffset = sf::Vector2f(0.f, radius + outsideNodeRadius);
	childrenNodes.push_back(FractalNode(fractal, halfMultiplier + 1, outsideNodeRadius, center + outsideNodeOffset, depth + 1, 0)); //Node 3 (top)
	childrenNodes.push_back(FractalNode(fractal, halfMultiplier + 1, outsideNodeRadius, center - outsideNodeOffset, depth + 1, 0)); //Node 4 (bottom)
}

void FractalNode::unloadChildren()
{
	childrenNodes.clear();
}

void FractalNode::draw(sf::RenderTarget& renderTarget)
{
	float zoom = radius * 2.f / cameraViewSize.x;
	if (zoom < renderZoomThreshold) return;
	if (zoom > loadThreshold && !childrenNodes.size()) loadChildren();
	float cullingYOffset = radius * 2.5f;
	if
		(!(
			center.x + radius > cameraBoundsLeft &&
			center.x - radius < cameraBoundsRight &&
			center.y + cullingYOffset > cameraBoundsTop &&
			center.y - cullingYOffset < cameraBoundsBottom
			))
	{
		return;
	}

	if (childrenNodes.size())
	{
		sf::Vector2f offsetA = sf::Vector2f(radius / 2.f, 0.f);
		childrenNodes[0].center = center + offsetA;
		childrenNodes[1].center = center - offsetA;
		sf::Vector2f offsetB = sf::Vector2f(0.f, radius * 1.25f);
		childrenNodes[2].center = center + offsetB;
		childrenNodes[3].center = center - offsetB;
	}

	else if (zoom <= unloadThreshold && childrenNodes.size()) unloadChildren();

	//shape.setPointCount(minRes + (maxRes - minRes) * std::clamp(zoom, 0.f, 1.f)); Dynamic resolution

	renderTarget.draw(shape);
	++Manager::renderCount;

	for (auto& child : childrenNodes)
	{
		child.draw(renderTarget);
	}
}

void FractalNode::updateCameraProperties()
{
	cameraView = Manager::getCameraView();
	cameraViewCenter = cameraView.getCenter();
	cameraViewSize = cameraView.getSize();
	scl = cameraViewSize / 2.f;

	cameraBoundsLeft = cameraViewCenter.x - scl.x;
	cameraBoundsRight = cameraViewCenter.x + scl.x;
	cameraBoundsTop = cameraViewCenter.y - scl.y;
	cameraBoundsBottom = cameraViewCenter.y + scl.y;
}
