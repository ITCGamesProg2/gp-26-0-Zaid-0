#include "Game.h"
#include <functional>
#include <iostream>
#include <stdexcept>

// Our target FPS
static float const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game(AssetManager& t_assetManager)
	: m_window(sf::VideoMode(sf::Vector2u(ScreenSize::s_width, ScreenSize::s_height)),
		"SFML Playground", sf::Style::Default),
	m_assetManager(t_assetManager),
	m_tank(t_assetManager, m_wallSprites),
	m_aiTank(m_assetManager.getTexture("tankAtlas"), m_wallSprites),
	m_bgSprite(m_tempTexture),
	m_hud(m_font)
#ifdef TEST_FPS
	, x_updateFPS(m_font),
	x_drawFPS(m_font)
#endif
{
	init();

	using std::placeholders::_1;
	m_funcApplyDamage = std::bind(&AITank::applyDamage, &m_aiTank, _1);
}

////////////////////////////////////////////////////////////
void Game::init()
{
	int currentLevel = 1;
	// Will generate an exception if level loading fails
	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch (std::exception& e)
	{
		std::cout << "Level Loading failure." << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}

	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);

	//Using YAML
	m_tank.setPosition(m_level.m_tank.m_position);
	m_tank.setScale(m_level.m_tank.m_scale);

	try
	{
		m_assetManager.loadFont("akashi", "resources/fonts/akashi.ttf");
		m_hudFont = m_assetManager.getFont("akashi");
	}
	catch (std::exception const& e)
	{
		std::cerr << "akashi font missing or invalid (" << e.what()
			<< "); using BebasNeue.otf if present.\n";
		if (!m_font.openFromFile("BebasNeue.otf"))
		{
			std::cerr << "Error loading fallback font file\n";
		}
		m_hudFont = m_font;
	}
	m_hud.getGameStateText().setFont(m_hudFont);

	m_assetManager.loadTexture("background", "resources/images/" + m_level.m_background.m_fileName);
	sf::Texture const& bgTexture = m_assetManager.getTexture("background");
	m_bgSprite.setTexture(bgTexture, true);

	// When the sprite is drawn without setTextureRect, the whole spritesheet is displayed
	// used setTextureRect to select only a specific portion of the texture (the blue tank body now)
	// BlueTank coordinates from sprites.txt: x=0, y=0, width=246, height=114

	// Set origin to center of sprite: ( width/2 , height/2) 

	// Move sprite using from level data

	// Set scale from level data

	// With the origin set to center, the sprite is now drawn with its center point at position (100, 100).
	// before the top-left corner was at (100, 100). Now the center of the sprite is at (100, 100).
	// Part of the sprite is missing because the center is at (100, 100), so the left and top portions extend


	// Rotate the sprite by 90 degrees
	// setRotation() sets the absolute rotation angle
	// Using SFML 3.0 sf::Angle type with sf::degrees()


	// The tank rotates clockwise with a positive angle
	// By default, 0 corresponds to "right" x+. Rotating 90 clockwise from right
	// means the sprite now points "down" y+.

	// rotate() adds to the current rotation 
	// If current rotation is 90, rotate(sf::degrees(45)) makes it 135
	// m_sprite.rotate(sf::degrees(45.0f)); 
	// This would add 45 to the current 90, making it 135

	// setRotation() sets the absolute rotation
	// setRotation(sf::degrees(90)) makes rotation to exactly 90, without caring about the previous value
	// m_sprite.setRotation(sf::degrees(180.0f));
	// This would set rotation to exactly 180 as not 90 + 180



	// Set up the turret sprite to show only the blue gun/turret
	// Gun Blue coordinates from sprites.txt: x=0, y=325, width=191, height=94

	// Set origin to center of turret sprite: (width/2, height/2) = (95.5, 47)
	// Position turret on top of the tank (centered horizontally, slightly above)
	// Updated position to match the new tank position
	// Rotate turret to match tank rotation

#ifdef TEST_FPS
	x_updateFPS.setFont(m_hudFont);
	x_updateFPS.setPosition(sf::Vector2f{ 20.0f, 300.0f });
	x_updateFPS.setCharacterSize(24);
	x_updateFPS.setFillColor(sf::Color::White);
	x_drawFPS.setFont(m_hudFont);
	x_drawFPS.setPosition(sf::Vector2f{ 20.0f, 350.0f });
	x_drawFPS.setCharacterSize(24);
	x_drawFPS.setFillColor(sf::Color::White);
#endif

	// When releaseTexture is called down, the texture is removed from the AssetManager.
	// the sprites are still referencing this texture, they crashed
	// because the texture data is no longer available. The sprites need the texture to remain
	// in memory as long as they are using it.
	
	//m_assetManager.releaseTexture("tankAtlas");
	generateWalls();
	m_aiTank.init(m_level.m_aiTank.m_position, m_level.m_aiTank.m_scale);
	m_aiTank.setOnEliminated([this]()
		{
			m_gameState = GameState::GAME_WIN;
			m_restartTimerMs = 0.0;
		});
}

void Game::restartRound()
{
	m_gameState = GameState::GAME_RUNNING;
	m_tank.resetRound(m_level.m_tank.m_position, m_level.m_tank.m_scale);
	m_aiTank.resetRound(m_level.m_aiTank.m_position, m_level.m_aiTank.m_scale);
}

void Game::generateWalls()
{
	// Wall coordinates from sprites.txt: x=192, y=325, width=32, height=22
	sf::IntRect wallRect{ sf::Vector2i{192, 325}, sf::Vector2i{32, 22} };
	sf::Texture const& texture = m_assetManager.getTexture("tankAtlas");

	for (auto const& obstacle : m_level.m_obstacles)
	{
		sf::Sprite sprite(texture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(
			sf::Vector2f{ 32.0f / 2.0f, 22.0f / 2.0f }
		);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}

////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	
	sf::Time timePerFrame = sf::seconds(1.0f / FPS); // 60 fps

	


	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame.asMilliseconds()); //60 fps
#ifdef TEST_FPS
			x_secondTime += timePerFrame;
			x_updateFrameCount++;
			if (x_secondTime.asSeconds() > 1)
			{
				std::string updatesPS = "UPS " + std::to_string(x_updateFrameCount - 1);
				x_updateFPS.setString(updatesPS);
				std::string drawsPS = "DPS " + std::to_string(x_drawFrameCount);
				x_drawFPS.setString(drawsPS);
				x_updateFrameCount = 0;
				x_drawFrameCount = 0;
				x_secondTime = sf::Time::Zero;
			}
#endif
		}
		render(); // as many as possible
#ifdef TEST_FPS
		x_drawFrameCount++;
#endif
	}
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{

	// get the next event from the event queue (if there is one)
	while (const std::optional event = m_window.pollEvent())
	{
		// "close requested" event: we close the window
		if (event->is<sf::Event::Closed>())
		{
			m_window.close();
		}
		if (event->is<sf::Event::KeyPressed>())
		{
			processKeyPressed(event);
		}		
	}
}

////////////////////////////////////////////////////////////
void Game::processKeyPressed(const std::optional<sf::Event>& t_event)
{	
	
	sf::Event::KeyPressed const* keyPressed = t_event->getIf<sf::Event::KeyPressed>();
	if (keyPressed != nullptr)
	{
		switch (keyPressed->code)
		{
			case sf::Keyboard::Key::Escape:
				m_window.close();
				break;
			case sf::Keyboard::Key::D:
				m_aiTank.setDebugMode(!m_aiTank.getDebugMode());
				break;
			default:
				break;
		}
	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	m_hud.update(m_gameState);

	if (m_gameState == GameState::GAME_RUNNING
		&& m_aiTank.collidesWithPlayer(m_tank))
	{
		m_gameState = GameState::GAME_LOSE;
		m_restartTimerMs = 0.0;
	}

	switch (m_gameState)
	{
	case GameState::GAME_RUNNING:
		m_tank.update(dt, m_funcApplyDamage, m_aiTank.getTankBase());
		m_aiTank.update(m_tank, dt);
		break;
	case GameState::GAME_WIN:
	case GameState::GAME_LOSE:
		m_restartTimerMs += dt;
		if (m_restartTimerMs >= s_restartDelayMs)
		{
			m_restartTimerMs = 0.0;
			restartRound();
		}
		break;
	default:
		break;
	}
}


////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	m_window.draw(m_bgSprite);
	// Draw tank body first, then turret on top
	//m_window.draw(m_sprite);
	//m_window.draw(m_turretSprite);

	// Draw tank
	m_tank.render(m_window);
	m_aiTank.render(m_window);
	// Draw wall 
	for (auto const& wall : m_wallSprites)
	{
		m_window.draw(wall);
	}

	m_hud.render(m_window);

#ifdef TEST_FPS
	m_window.draw(x_updateFPS);
	m_window.draw(x_drawFPS);
#endif
	m_window.display();
}














