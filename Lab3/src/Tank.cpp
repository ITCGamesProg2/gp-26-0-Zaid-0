#include "Tank.h"
#include <iostream>

Tank::Tank(AssetManager & t_assetManager)
  : m_assetManager(t_assetManager),
    m_tankBase(m_assetManager.getTexture("tankAtlas")),
	m_turret(m_assetManager.getTexture("tankAtlas"))
{
	initSprites();
}

void Tank::update(double dt)
{	
	m_speed = std::clamp(m_speed, MAX_REVERSE_SPEED, MAX_FORWARD_SPEED);
	// calculate new position based on rotation and speed
	double rotationRadians = m_rotation.asRadians(); //convert to radius to use in sin-cos

	sf::Vector2f currentPos = m_tankBase.getPosition();	// current position

	// new position: current + cos(rotation) * speed * (dt / 1000)
	// dt is in milliseconds, so divide by 1000 to get seconds
	double dtSeconds = dt / 1000.0;
	double newX = currentPos.x + std::cos(rotationRadians) * m_speed * dtSeconds;
	double newY = currentPos.y + std::sin(rotationRadians) * m_speed * dtSeconds;

	// tank base to new position
	m_tankBase.setPosition(sf::Vector2f{ static_cast<float>(newX), static_cast<float>(newY) });

	//update rotation angle of tank base
	m_tankBase.setRotation(m_rotation);

	// (turret follows tank position)
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_rotation);

	// apply friction to gradually slow down the tank
	m_speed *= 0.99;
}

void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_tankBase);
	window.draw(m_turret);
}

void Tank::setPosition(sf::Vector2f t_position)
{
	m_tankBase.setPosition(t_position);
	m_turret.setPosition(t_position);
}

sf::Vector2f Tank::getPosition()
{
	return m_tankBase.getPosition();
}


void Tank::setScale(sf::Vector2f t_scale)
{
	m_tankBase.setScale(t_scale);
	m_turret.setScale(t_scale);
}

void Tank::setScale(double t_scale)
{
	float scale = static_cast<float>(t_scale);
	m_tankBase.setScale(sf::Vector2f{ scale, scale }); 
	m_turret.setScale(sf::Vector2f{ scale, scale }); 
}

void Tank::increaseSpeed()
{
	m_speed += 1;
}

void Tank::decreaseSpeed()
{
	m_speed -= 1;
}

void Tank::increaseRotation()
{
	m_rotation += sf::degrees(1.0f);
	if (m_rotation.asDegrees() == 360.0f)
	{
		m_rotation = sf::degrees(0.0);
	}
}

void Tank::decreaseRotation()
{
	m_rotation -= sf::degrees(1.0f);
	if (m_rotation.asDegrees() == 0.0f)
	{
		m_rotation = sf::degrees(359.0f);
	}
}

void Tank::initSprites()
{
	// Set texture rectangle for tank base
	sf::IntRect tankBaseRect{ sf::Vector2i{0, 0}, sf::Vector2i{246, 114} };
	m_tankBase.setTextureRect(tankBaseRect);
	// set origin of tank base to center (mounting point for turret)
	// origin at center
	m_tankBase.setOrigin(sf::Vector2f{ 123.0f, 57.0f });

	//set texture rectangle for turret
	sf::IntRect turretRect{ sf::Vector2i{0, 325}, sf::Vector2i{191, 94} };
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(sf::Vector2f{ 95.5f, 50.0f });

}

