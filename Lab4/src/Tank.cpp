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
	//process keyboard input first to handle key presses
	handleKeyInput();

	if (m_centringTurret)
	{
		centreTurret();
	}

	m_speed = std::clamp(m_speed, MAX_REVERSE_SPEED, MAX_FORWARD_SPEED); //clamp used to limit the speed


	// 2.3.1::  now yes, the tank moves in response to keyboard input because,
	// - processKeyPressed() calls speed and rotation functions 
	// - update() function is called every frame in the game loop

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
	m_turret.setRotation(m_rotation + m_turretRotation);

	// apply friction to gradually slow down the tank
	m_speed *= 0.99;
	// notice multiplying by 0.99 reduces the value by 1% each frame, causing stopping at the end and approaching 0


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
	m_tankBase.setOrigin(sf::Vector2f{ 90.0f, 57.0f });

	//set texture rectangle for turret
	sf::IntRect turretRect{ sf::Vector2i{0, 325}, sf::Vector2i{191, 94} };
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(sf::Vector2f{ 50.0f, 50.0f });

}


void Tank::handleKeyInput()
{

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
	{
		increaseSpeed();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
	{
		decreaseSpeed();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		increaseRotation();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		decreaseRotation();
	}
	// Turret rotation controls (Z and X keys)
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
	{
		increaseTurretRotation();
		//stop centring if user manually rotates turret
		if (m_centringTurret)
		{
			m_centringTurret = false;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X))
	{
		decreaseTurretRotation();
		//same as Z
		if (m_centringTurret)
		{
			m_centringTurret = false;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C))
	{
		setCentringTurret(true);
	}
}

////////////////////////////////////////////////////////////
void Tank::increaseTurretRotation()
{
	m_turretRotation += sf::degrees(1.0);
	if (m_turretRotation.asDegrees() >= 360.0)
	{
		m_turretRotation = sf::degrees(m_turretRotation.asDegrees() - 360.0);
	}
}

////////////////////////////////////////////////////////////
void Tank::decreaseTurretRotation()
{
	m_turretRotation -= sf::degrees(1.0);
	double degrees = m_turretRotation.asDegrees();
	if (degrees < 0.0)
	{
		m_turretRotation = sf::degrees(degrees + 360.0);
	}
}

///////////////////////////////////////////////////////////
void Tank::setCentringTurret(bool t_centring)
{
	m_centringTurret = t_centring;
}


void Tank::centreTurret()
{
	// A = turret direction
	//total turret rotation now = tank base rotation + turret relative rotation
	sf::Angle totalTurretRotation = m_rotation + m_turretRotation; //as degrees 
	double turretRadians = totalTurretRotation.asRadians();
	double turretX = std::cos(turretRadians);
	double turretY = std::sin(turretRadians);

	//B = tank base direction
	double tankRadians = m_rotation.asRadians();
	double tankX =std::cos(tankRadians); 
	double tankY = std::sin(tankRadians);

	// get cross = (A.x * B.y) - (A.y * B.x)
	double cross = (turretX * tankY) - (turretY * tankX);

	// epsilon value to prevent jittering when almost centered
	const double epsilon = 0.01;

	//if turret is already centered
	if (std::abs(cross) < epsilon)
	{
		//ؤlamp to exactly 0 and stop centring
		cross = 0.0;
		m_turretRotation = sf::degrees(0.0);
		m_centringTurret = false;
	}
	else if (cross > epsilon)
	{
		increaseTurretRotation(); //centre it
	}
	else // cross less than -epsilon : diffrent side
	{
		decreaseTurretRotation();
	}
}

