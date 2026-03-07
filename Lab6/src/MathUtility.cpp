#include "MathUtility.h"

namespace MathUtility
{
	double distance(sf::Vector2f v1, sf::Vector2f v2)
	{
		return (v2 - v1).length();
	}

	bool lineIntersectsCircle(sf::Vector2f ahead, sf::Vector2f halfAhead, sf::CircleShape circle)
	{
		return distance(circle.getPosition(), ahead) <= circle.getRadius() ||
			distance(circle.getPosition(), halfAhead) <= circle.getRadius();
	}

	sf::Vector2f truncated(sf::Vector2f vector, float maxForce)
	{
		float lengthSq = vector.x * vector.x + vector.y * vector.y;
		if (lengthSq > maxForce * maxForce)
		{
			return (vector / std::sqrt(lengthSq)) * maxForce;
		}
		return vector;
	}
}
