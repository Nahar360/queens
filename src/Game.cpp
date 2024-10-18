#include "Game.hpp"

#include "GlobalSettings.hpp"

CGame::CGame()
    : m_window(sf::VideoMode(GlobalSettings::WINDOW_WIDTH, GlobalSettings::WINDOW_HEIGHT), GlobalSettings::WINDOW_TITLE)
{
    srand(time(NULL));
}

void CGame::Run()
{
    // Initialisation
    m_uiManager.Init(m_window);
    m_world.Init();

    sf::Clock clock = sf::Clock();
    sf::Time previousTime = clock.getElapsedTime();
    sf::Time currentTime;

    // Game loop
    while (m_window.isOpen())
    {
        currentTime = clock.getElapsedTime();
        const float fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds());
        previousTime = currentTime;

        CheckMouseHover();

        CheckEvents();

        m_uiManager.Run(m_window, m_world, fps);

        m_window.clear(GlobalSettings::BACKGROUND_COLOR);

        m_world.Update(m_window);

        m_uiManager.Render(m_window);

        m_window.display();
    }

    m_uiManager.Shutdown();
}

void CGame::CheckMouseHover()
{
    // We are only interested on hover events if the world has been loaded
    if (m_world.HasLoaded())
    {
        const sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
        m_world.ChangeHoveredTileColor(mousePos);
    }
}

void CGame::CheckEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        m_uiManager.ProcessEvent(event);

        if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            m_window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            m_world.MouseDetection(event.mouseButton.button, sf::Mouse::getPosition(m_window));
        }
    }
}
