#include "AITank.h"

////////////////////////////////////////////////////////////
AITank::AITank(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites)
	: m_aiBehaviour(AiBehaviour::SEEK_PLAYER)
	, m_texture(texture)
	, m_wallSprites(wallSprites)
	, m_steering(0, 0)
{
	
	// Initialises the tank base and turret sprites.
	initSprites();
}



////////////////////////////////////////////////////////////
void AITank::update(Tank const & playerTank, double dt)
{
	m_steering = sf::Vector2f(0, 0); 

	sf::Vector2f vectorToPlayer = seek(playerTank.getPosition());	
	switch (m_aiBehaviour)
	{
		case AiBehaviour::SEEK_PLAYER:
			m_steering += vectorToPlayer.normalized();
			m_steering += collisionAvoidance();
			m_steering = MathUtility::truncated(m_steering, MAX_FORCE);
			m_velocity = MathUtility::truncated(m_velocity + m_steering, MAX_SPEED);
			break;
	
		case AiBehaviour::STOP:
			m_velocity = sf::Vector2f(0, 0);
			break;
		default:
			break;
	}
	
	// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
	// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
	// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).

	if (m_velocity.x != 0 || m_velocity.y != 0) 
	{		
		// Get the angle directly from the vector 
		sf::Angle dest = m_velocity.angle();

		// SFML 3 provides a wrapSigned function: Wrap to a range such that -180° <= angle < 180° 
		// Here we get the tank to rotate a small amount (10%) of the way towards the desired angle each frame, which creates a smooth turning effect.
		m_rotation += (dest - m_rotation).wrapSigned() * 0.1f;
	}

	if (vectorToPlayer.length() < MAX_SEE_AHEAD)
	{
		m_aiBehaviour = AiBehaviour::STOP;
	}
	else
	{
		m_aiBehaviour = AiBehaviour::SEEK_PLAYER;
	}

	updateMovement(dt);

}

////////////////////////////////////////////////////////////
void AITank::render(sf::RenderWindow & window)
{
	// TODO: Don't draw if off-screen...
	window.draw(m_tankBase);
	window.draw(m_turret);	
}

////////////////////////////////////////////////////////////
void AITank::init(sf::Vector2f t_position, sf::Vector2f t_scale)
{
	m_tankBase.setPosition(t_position);
	m_tankBase.setScale(sf::Vector2f{t_scale.x, t_scale.y});
	m_turret.setPosition(t_position);
	m_turret.setScale(sf::Vector2f{t_scale.x, t_scale.y});

	for (sf::Sprite const wallSprite : m_wallSprites)
	{
		sf::CircleShape circle(wallSprite.getTextureRect().size.x * 1.5f); 
		circle.setOrigin(sf::Vector2f{ circle.getRadius(), circle.getRadius() });
		circle.setPosition(wallSprite.getPosition());
		m_obstacles.push_back(circle);
	}
}

////////////////////////////////////////////////////////////
sf::Vector2f AITank::seek(sf::Vector2f t_playerPosition) const
{
	// Vector pointing from AI tank towards player tank
	return t_playerPosition - m_tankBase.getPosition();
		
}

////////////////////////////////////////////////////////////
sf::Vector2f AITank::collisionAvoidance()
{
	auto headingRadians = m_rotation.asRadians();
	sf::Vector2f headingVector(std::cos(headingRadians) * MAX_SEE_AHEAD, std::sin(headingRadians) * MAX_SEE_AHEAD);
	m_ahead = m_tankBase.getPosition() + headingVector;
	m_halfAhead = m_tankBase.getPosition() + (headingVector * 0.5f);

	//compute left and right lookahead vectors 45deg at half the ahead length
	sf::Vector2f halfHeading = headingVector * 0.5f;
	sf::Vector2f left = halfHeading.rotatedBy(sf::degrees(-45));
	sf::Vector2f right = halfHeading.rotatedBy(sf::degrees(45));
	m_aheadLeft = m_tankBase.getPosition() + left;
	m_aheadRight = m_tankBase.getPosition() + right;

	sf::Vector2f collisionVector = findMostThreateningObstacle();

	sf::Vector2f avoidance(0, 0);

	// If there is a threatening obstacle, calculate an avoidance vector which is a unit vector pointing away from the obstacle multiplied by 
	// the maximum avoidance force. 
	// If there is no threatening obstacle, then the avoidance vector should be (0,0).
	if (m_mostThreatening.getRadius() != 0.0f)
	{				
		// Slow down slightly while avoiding
		m_velocity *= 0.95f; 
		// The closer the obstacle, the stronger the avoidance force will be.
		float dist = MathUtility::distance(m_tankBase.getPosition(), m_mostThreatening.getPosition());

		// Scale factor: 1.0 when very close, 0.0 when at MAX_SEE_AHEAD
		float multiplier = 1.0f + (MAX_SEE_AHEAD - dist) / MAX_SEE_AHEAD;
		avoidance = collisionVector - m_mostThreatening.getPosition();
		avoidance = avoidance.normalized() * MAX_AVOID_FORCE * multiplier;
	}
	else
	{		
		avoidance *= 0.0f;
	}
	
	return avoidance;
}

sf::Vector2f AITank::findMostThreateningObstacle() 
{
	sf::Vector2f collisionVector(0.0f, 0.0f);
	sf::CircleShape mostThreatening{ 0.0f };

	for (sf::CircleShape & circle : m_obstacles)
	{
		bool collision = MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, circle);

		// if not, also test the left and right lookahead vectors using their midpoints as halfahead points
		if (!collision)
		{
			sf::Vector2f halfLeft = m_tankBase.getPosition() + (m_aheadLeft - m_tankBase.getPosition()) * 0.5f;
			collision = MathUtility::lineIntersectsCircle(m_aheadLeft, halfLeft, circle);
			if (collision)
			{
				// Collision came from the left look ahead
				if (mostThreatening.getRadius() == 0.0f ||
					MathUtility::distance(m_tankBase.getPosition(), circle.getPosition()) <
					MathUtility::distance(m_tankBase.getPosition(), mostThreatening.getPosition()))
				{
					mostThreatening = circle;
					collisionVector = m_aheadLeft;
				}
				continue;
			}
		}

		if (!collision)
		{
			sf::Vector2f halfRight = m_tankBase.getPosition() + (m_aheadRight - m_tankBase.getPosition()) * 0.5f;
			collision = MathUtility::lineIntersectsCircle(m_aheadRight, halfRight, circle);
			if (collision)
			{
				// Collision came from the right look ahead
				if (mostThreatening.getRadius() == 0.0f ||
					MathUtility::distance(m_tankBase.getPosition(), circle.getPosition()) <
					MathUtility::distance(m_tankBase.getPosition(), mostThreatening.getPosition()))
				{
					mostThreatening = circle;
					collisionVector = m_aheadRight;
				}
				continue;
			}
		}

		if (collision)
		{
			if (mostThreatening.getRadius() == 0.0f ||
				MathUtility::distance(m_tankBase.getPosition(), circle.getPosition()) <
				MathUtility::distance(m_tankBase.getPosition(), mostThreatening.getPosition()))
			{
				mostThreatening = circle;
				collisionVector = m_ahead;
			}
		}
	}
	m_mostThreatening = mostThreatening;
	return collisionVector;
}

////////////////////////////////////////////////////////////
void AITank::initSprites()
{
	// Brown tank base from sprites.txt: BrownTank,0,115,244,114
	sf::IntRect brownTankRect({ 0, 115 }, { 244, 114 });
	m_tankBase.setTexture(m_texture);
	m_tankBase.setTextureRect(brownTankRect);

	// Set origin roughly at the turret mounting point (similar إلى اللاعب)
	m_tankBase.setOrigin(sf::Vector2f{ 90.0f, 57.0f });

	// Initialise the turret
	m_turret.setTexture(m_texture);
	// Brown gun from sprites.txt: Gun_01_Brown,0,230,212,94
	sf::IntRect turretRect({ 0, 230 }, { 212, 94 });
	m_turret.setTextureRect(turretRect);

	m_turret.setOrigin(sf::Vector2f{ 50.0f, 47.0f });
}

////////////////////////////////////////////////////////////
void AITank::updateMovement(double dt)
{
	// Move the tank using the velocity vector.
	m_tankBase.move(m_velocity * static_cast<float>(dt / 1000.0));
	m_tankBase.setRotation(m_rotation);
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_rotation);
}



