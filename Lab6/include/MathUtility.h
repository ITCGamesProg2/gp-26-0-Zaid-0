#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

namespace MathUtility
{
	double distance(sf::Vector2f v1, sf::Vector2f v2);

	bool lineIntersectsCircle(sf::Vector2f ahead, sf::Vector2f halfAhead, sf::CircleShape circle);

	// Truncate a vector to a maximum magnitude. 
	// This is used to limit the steering and velocity vectors to their maximum values.
	sf::Vector2f truncated(sf::Vector2f t_vector, float t_maxForce);
}
