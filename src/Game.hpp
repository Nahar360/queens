#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include "UiManager.hpp"
#include "World.hpp"

class CGame
{
public:
    CGame();
    ~CGame() = default;

    void Run();

private:
    sf::RenderWindow m_window;

    CUiManager m_uiManager;
    CWorld m_world;

    bool m_mouseHasBeenPressed = false;

    void CheckEvents();
};

#endif // GAME_HPP
