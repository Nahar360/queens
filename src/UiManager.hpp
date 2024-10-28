#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include <vector>

#include <SFML/Graphics.hpp>

#include "Level.hpp"

class UiManager
{
public:
    UiManager() = default;
    ~UiManager() = default;

    void Init(sf::RenderWindow& window);
    void Shutdown();

    std::vector<std::string> getLevelsToLoad() const { return m_levelsToLoad; }

    void ProcessEvent(sf::Event event);

    void Run(sf::RenderWindow& window, Level& level, float fps);
    void Render(sf::RenderWindow& window);

private:
    sf::Clock m_deltaClock;

    std::vector<std::string> m_levelsToLoad;

    // UI
    void Update(sf::RenderWindow& window);
    void Begin();
    void HandleUi(sf::RenderWindow& window, Level& level, float fps);
    void End();

    void Reload();

    void UpdateWindowTitle(sf::RenderWindow& window);
    void ShowFPS(float fps);
    void UpdateMousePosition(sf::RenderWindow& window);
    void UpdateBackgroundColor();

    void ShowRules();

    void DebugUi(Level& level);

    // Level related
    void LoadLevel(Level& level);
    void ResetLevel(Level& level);
    void SolveLevel(Level& level);

    void ShowElapsedTime(Level& level);
    void ShowLevelCompleted();

    // Non-UI helper functions
    void GetLevelsToLoad();
};

#endif // UI_MANAGER_HPP
