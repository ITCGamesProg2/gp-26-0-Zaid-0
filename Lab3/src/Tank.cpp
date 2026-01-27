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
	m_tankBase.setScale(sf::Vector2f{ t_scale, t_scale });
	m_turret.setScale(sf::Vector2f{ t_scale, t_scale });
}

void Tank::initSprites()
{
	

}

