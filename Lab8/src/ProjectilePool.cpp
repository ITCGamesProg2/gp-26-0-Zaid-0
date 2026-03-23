#include "ProjectilePool.h"

////////////////////////////////////////////////////////////
void ProjectilePool::create(sf::Texture const& t_texture, double t_x, double t_y, sf::Angle t_rotation)
{
	// prefer an available projectile
	int selectedIndex = m_nextAvailable;
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

	m_projectiles.at(selectedIndex).init(t_texture, t_x, t_y, t_rotation);
	m_nextAvailable = (selectedIndex + 1) % s_POOL_SIZE;
}

////////////////////////////////////////////////////////////
void ProjectilePool::update(double t_dt, std::vector<sf::Sprite>& t_wallSprites,
	std::function<void(int)>& t_funcApplyDamage, sf::Sprite const& t_aiTankBase) 
{
	int activeCount = 0;
	int firstAvailable = -1;

	for (int i = 0; i < s_POOL_SIZE; ++i)
	{
		const bool stillActive = m_projectiles.at(i).update(t_dt, t_wallSprites, t_funcApplyDamage, t_aiTankBase);
		if (stillActive)
		{
			++activeCount;
		}
		else if (firstAvailable < 0)
		{
			firstAvailable = i;
		}
	}

	m_poolFull = (activeCount == s_POOL_SIZE);
	if (!m_poolFull && firstAvailable >= 0)
	{
		m_nextAvailable = firstAvailable;
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
///////////////////////////////////////////
void ProjectilePool::reset()
{
	for (Projectile& p : m_projectiles)
	{
		p.deactivate();
	}
	m_nextAvailable = 0;
	m_poolFull = false;
}