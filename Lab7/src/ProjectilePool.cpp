#include "ProjectilePool.h"

////////////////////////////////////////////////////////////
void ProjectilePool::create(sf::Texture const & t_texture, double t_x, double t_y, sf::Angle t_rotation)
{
	// Find the next free projectile if possible; otherwise reuse in a round-robin fashion.
	int selectedIndex = m_nextAvailable;

	if (!m_poolFull)
	{
		bool foundFree = false;
		for (int tries = 0; tries < s_POOL_SIZE; ++tries)
		{
			const int idx = (m_nextAvailable + tries) % s_POOL_SIZE;
			if (!m_projectiles.at(idx).inUse())
			{
				selectedIndex = idx;
				foundFree = true;
				break;
			}
		}
		m_poolFull = !foundFree;
	}
	
	m_projectiles.at(selectedIndex).init(t_texture, t_x, t_y, t_rotation);
	m_nextAvailable = (selectedIndex + 1) % s_POOL_SIZE;
}

////////////////////////////////////////////////////////////t_
void ProjectilePool::update(double t_dt, std::vector<sf::Sprite> & t_wallSprites)
{	
	// The number of active projectiles.
	int activeCount = 0;
	// Assume the pool is not full initially.
	m_poolFull = false;
	for (int i = 0; i < s_POOL_SIZE; i++)
	{
		// If m_projectiles.at(i).update() returns true, then this projectile is onscreen
		// Note how the condition is negated (i.e. checking if projectile is not in use)
		if( !m_projectiles.at(i).update(t_dt, t_wallSprites))
		{
			// If this projectile has expired, make it the next available.
			m_nextAvailable = i;
		}
		else
		{
			// So we know how many projectiles are active.
			activeCount++;
		}
	}
	// If no projectiles available, set a flag.
	if (s_POOL_SIZE == activeCount)
	{		
		m_poolFull = true;
	}
}

////////////////////////////////////////////////////////////
void ProjectilePool::render(sf::RenderWindow & t_window)
{
	for (int i = 0; i < s_POOL_SIZE; i++)
	{
		// If projectile is active...
		if (m_projectiles.at(i).inUse())
		{
			t_window.draw(m_projectiles.at(i).m_projectile);
		}
	}
}