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

    void Init();
    void Run();
    void Shutdown();

private:
    sf::RenderWindow m_window;

    CUiManager m_uiManager;
    CWorld m_world;

    bool m_mouseHasBeenPressed = false;

    void CheckMouseHover();
    void CheckEvents();
};

#endif // GAME_HPP
