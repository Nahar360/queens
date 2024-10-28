#include "Game.hpp"

#include "GlobalSettings.hpp"
#include "ResourceManager.hpp"

Game::Game()
    : m_window(sf::VideoMode(GlobalSettings::WINDOW_WIDTH, GlobalSettings::WINDOW_HEIGHT), GlobalSettings::WINDOW_TITLE)
{
    srand(time(NULL));
}

void Game::Init()
{
    ResourceManager::getInstance().Load();

    m_uiManager.Init(m_window);

    // Load the first level
    const std::vector<std::string> levelsToLoad = m_uiManager.getLevelsToLoad();
    m_level.Init(levelsToLoad[0]);
}

void Game::Run()
{
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

        m_uiManager.Run(m_window, m_level, fps);

        m_window.clear(GlobalSettings::BACKGROUND_COLOR);

        m_level.Update(m_window);

        m_uiManager.Render(m_window);

        m_window.display();
    }
}

void Game::Shutdown()
{
    m_uiManager.Shutdown();
}

void Game::CheckMouseHover()
{
    // We are only interested on hover events if the level has been loaded
    if (m_level.HasLoaded())
    {
        const sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
        m_level.ChangeHoveredTileColor(mousePos);
    }
}

void Game::CheckEvents()
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
            m_level.MouseDetection(event.mouseButton.button, sf::Mouse::getPosition(m_window));
        }
    }
}
