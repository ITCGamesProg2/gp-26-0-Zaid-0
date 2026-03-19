#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.h"


/// <summary>
/// @brief A basic HUD implementation.
/// 
/// Shows current game state only.
/// </summary>

class HUD
{
public:
    /// <summary>
    /// @brief Constructor that stores a font for the HUD and initialises the general HUD appearance.
    /// </summary>
    HUD(sf::Font & t_font);

    /// <summary>
    /// @brief Checks the current game state and sets the appropriate status text on the HUD.
    /// </summary>
    /// <param name="gameState">The current game state</param>
    void update(GameState const& gameState);

    /// <summary>
    /// @brief Draws the HUD outline and text.
    ///
    /// </summary>
    /// <param name="window">The SFML Render window</param>
    void render(sf::RenderWindow& window);

    sf::Text& getGameStateText();

private: 

    // A container for the current HUD text.
    sf::Text m_gameStateText;

    // A simple background shape for the HUD.
    sf::RectangleShape m_hudOutline;
	
};