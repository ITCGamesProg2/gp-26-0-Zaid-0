#pragma once
#include <numbers>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Tank.h"
#include "MathUtility.h"


class AITank
{
public:
	/// <summary>
	/// @brief Constructor that stores a reference to the obstacle container.
	/// Initialises steering behaviour to seek (player) mode, sets the AI tank position and
	///  initialises the steering vector to (0,0) meaning zero force magnitude.
	/// </summary>
	/// <param name="texture">A reference to the sprite sheet texture</param>
	///< param name="wallSprites">A reference to the container of wall sprites</param>
	AITank(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites);

	/// <summary>
	/// @brief Steers the AI tank towards the player tank avoiding obstacles along the way.
	/// Gets a vector to the player tank and sets steering and velocity vectors towards
	/// the player if current behaviour is seek. If behaviour is stop, the velocity vector
	/// is set to 0. Then compute the correct rotation angle to point towards the player tank. 
	/// If the distance to the player tank is < MAX_SEE_AHEAD, then the behaviour is changed from seek to stop.
	/// Finally, recalculate the new position of the tank base and turret sprites.
	/// </summary>
	/// <param name="playerTank">A reference to the player tank</param>
	/// <param name="dt">update delta time</param>
	void update(Tank const & playerTank, double dt);

	/// <summary>
	/// @brief Draws the tank base and turret.
	///
	/// </summary>
	/// <param name="window">The SFML Render window</param>
	void render(sf::RenderWindow & window);

	/// <summary>
	/// @brief Initialises the obstacle container and sets the tank base/turret sprites to the specified position and scale.
	/// <param name="t_position">An x,y position</param>
	/// <param name="t_scale">A scaling factor to be applied in the x,y dimensions</param>
	/// </summary>
	void init(sf::Vector2f t_position, sf::Vector2f t_scale);

	enum class AiType
	{
		AI_ID_NONE,
		AI_ID_SEEK_SHOOT_AT_PLAYER
	};

private:
	

	void initSprites();

	void updateMovement(double dt);

	sf::Vector2f seek(sf::Vector2f t_playerPosition) const;

	sf::Vector2f collisionAvoidance();

	sf::Vector2f findMostThreateningObstacle();

	// A reference to the sprite sheet texture.
	sf::Texture const & m_texture;

	// A sprite for the tank base.
	sf::Sprite m_tankBase{ m_texture };

	// A sprite for the turret
	sf::Sprite m_turret{ m_texture };

	// A reference to the container of wall sprites.
	std::vector<sf::Sprite> & m_wallSprites;

	// The current rotation as applied to tank base and turret.
	sf::Angle m_rotation{ sf::degrees(0.0) };

	// Current velocity.
	sf::Vector2f m_velocity;

	// Steering vector.
	sf::Vector2f m_steering;

	// The ahead vector.
	sf::Vector2f m_ahead;

	// The half-ahead vector.
	sf::Vector2f m_halfAhead;

	// The ahead vector looking 45 degrees left.
	sf::Vector2f m_aheadLeft;

	// The ahead vector looking 45 degrees right.
	sf::Vector2f m_aheadRight;

	sf::Vector2f m_projectedVelocity;

	// The maximum see ahead range.
	static float constexpr MAX_SEE_AHEAD{ 150.0f };

	// The maximum avoidance turn rate.
	static float constexpr MAX_AVOID_FORCE{ 50.0f};

	// The maximum turning force.
	static float constexpr MAX_FORCE{ 10.0f };

	// The maximum speed for this tank.
	float MAX_SPEED = 50.0f;

	
	// A container of circles that represent the obstacles to avoid.
	std::vector<sf::CircleShape> m_obstacles;

	enum class AiBehaviour
	{
		SEEK_PLAYER,		
		STOP,
		RETREAT
	} m_aiBehaviour;


	sf::CircleShape m_mostThreatening{ 0.0f };

	sf::RectangleShape m_aheadVector;
	sf::RectangleShape m_aheadLeftVector;
	sf::RectangleShape m_aheadRightVector;

	// When true, draw ahead vectors and obstacle circles
	bool m_debugMode{ false };

public:
	void setDebugMode(bool t_enabled) { m_debugMode = t_enabled; }
	bool getDebugMode() const { return m_debugMode; }
};

