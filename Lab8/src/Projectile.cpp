#include "Projectile.h"

////////////////////////////////////////////////////////////
void Projectile::init(sf::Texture const & t_texture, double t_x, double t_y, sf::Angle t_rotation)
{	
	m_projectile.setTexture(t_texture);
	m_projectile.setTextureRect(m_projectileRect);
	m_projectile.setOrigin(sf::Vector2f(m_projectileRect.size.x / 2.0, m_projectileRect.size.y/ 2.0));
	m_projectile.setPosition(sf::Vector2f(t_x, t_y));	
	m_projectile.setRotation(t_rotation);
	m_speed = s_MAX_SPEED;
}

////////////////////////////////////////////////////////////
bool Projectile::update(double t_dt, std::vector<sf::Sprite> & t_wallSprites)
{
	if (!inUse())
	{
		// If this projectile is not in use, there is no update routine to perform.
		return false;
	}
	
	sf::Vector2f position = m_projectile.getPosition();
	sf::Vector2f newPos(position.x + std::cos(m_projectile.getRotation().asRadians()) * m_speed * (t_dt / 1000),
		position.y + std::sin(m_projectile.getRotation().asRadians()) * m_speed * (t_dt / 1000));

	m_projectile.setPosition(sf::Vector2f(newPos.x, newPos.y));

	if (!isOnScreen(newPos)) 
	{
		m_speed = 0;	
	}
	else 
	{
		// Still on-screen, have we collided with a wall?
		for (sf::Sprite const & sprite : t_wallSprites)
		{
			// Checks if the projectile has collided with the current wall sprite.
			if (CollisionDetector::collision(m_projectile, sprite)) 
			{
				m_speed = 0;
			}
		}		
	}
	return inUse();
}

////////////////////////////////////////////////////////////
bool Projectile::inUse() const 
{ 
	// Projectile is not in use if not moving.
	return m_speed > 0.0;
}

////////////////////////////////////
void Projectile::deactivate()
{
	m_speed = 0.0;
}

////////////////////////////////////////////////////////////
bool Projectile::isOnScreen(sf::Vector2f t_position) const
{	
	return t_position.x - m_projectileRect.size.x / 2 > 0.f
		&& t_position.x + m_projectileRect.size.x / 2 < ScreenSize::s_width
	    && t_position.y - m_projectileRect.size.y / 2 > 0.f
		&& t_position.y + m_projectileRect.size.y / 2 < ScreenSize::s_height;
}