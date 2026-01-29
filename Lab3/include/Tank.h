#pragma once
#include <SFML/Graphics.hpp>
#include "AssetManager.h"

/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
	/// <summary>
	/// @brief Constructor function to initialise sprite members.
	/// <param name="t_assetManager">The asset manager initialised with the texture atlas</param>
	/// </summary>
	Tank(AssetManager & t_assetManager);
	
	/// <summary>
	/// @brief Handle all the movement and general update logic for the tank.
	/// <param name="dt">The time between frames in milliseconds</param>
	/// </summary>
	void update(double dt);
	
	/// <summary>
	/// @brief To draw the turret and tank base.
	/// <param name="window">The SFML render window</param>
	/// </summary>
	void render(sf::RenderWindow & window);
	
	/// <summary>
	/// @brief Simple setter for the tank base and turret position
	/// <param name="t_position">A position value</param>
	/// </summary>
	void setPosition(sf::Vector2f t_position);
	
	sf::Vector2f getPosition();
	
	/// <summary>
	/// @brief Scaling factor to apply to the tank base and turret
	/// <param name="t_scale">The x,y scaling value</param>
	/// </summary>
	void setScale(sf::Vector2f t_scale);

	/// <summary>
	/// @brief (overload) Scaling factor to apply to the tank base and turret
	/// <param name="t_scale">A single value representing the combined x,y scaling factor</param>
	/// </summary>
	void setScale(double t_scale);
	void increaseSpeed();
	void decreaseSpeed();
	void increaseRotation();
	void decreaseRotation();
	
private:	
	void initSprites();
	
	AssetManager & m_assetManager;
	sf::Sprite m_tankBase;
	sf::Sprite m_turret;

	// The tank speed.
	double m_speed{ 0.0 };

	// The current rotation as applied to tank base.
	sf::Angle m_rotation{ sf::degrees(0.0) };

	static double constexpr MAX_FORWARD_SPEED{ 100.0 };
	static double constexpr MAX_REVERSE_SPEED{ -100.0 };
};
