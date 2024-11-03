#ifndef GAME_HPP
#define GAME_HPP

#include "SFML/Graphics/RenderWindow.hpp"

#include "Level.hpp"
#include "UiManager.hpp"

class Game
{
public:
    Game();
    ~Game() = default;

    void Init();
    void Run();
    void Shutdown();

private:
    sf::RenderWindow m_window;

    UiManager m_uiManager;
    Level m_level;

    bool m_mouseHasBeenPressed = false;

    void CheckMouseHover();
    void CheckEvents();
};

#endif // GAME_HPP
