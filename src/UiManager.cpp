#include "UiManager.hpp"

#include <algorithm> // for std::sort
#include <climits>
#include <filesystem>
#include <string>

#include "imgui-SFML.h"
#include "imgui.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "GlobalSettings.hpp"
#include "Level.hpp"
#include "UiSettings.hpp"

void UiManager::Init(sf::RenderWindow& window)
{
    ImGui::SFML::Init(window);

    GetLevelsToLoad();
}

void UiManager::Shutdown()
{
    ImGui::SFML::Shutdown();
}

void UiManager::ProcessEvent(sf::Event event)
{
    ImGui::SFML::ProcessEvent(event);
}

void UiManager::Update(sf::RenderWindow& window)
{
    ImGui::SFML::Update(window, m_deltaClock.restart());
}

void UiManager::Begin()
{
    ImGui::Begin("Menu");
}

void UiManager::Run(sf::RenderWindow& window, Level& level, float fps)
{
    Update(window);

    Begin();

    HandleUi(window, level, fps);

    End();
}

void UiManager::HandleUi(sf::RenderWindow& window, Level& level, float fps)
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "General settings");

    UpdateWindowTitle(window);

    ShowFPS(fps);

    UpdateMousePosition(window);

    UpdateBackgroundColor();

    // -------------------------
    ImGui::Separator();
    ImGui::Separator();
    // -------------------------

    DebugUi(level);

    // -------------------------
    ImGui::Separator();
    ImGui::Separator();
    // -------------------------

    LoadLevel(level);

    // -------------------------
    ImGui::Separator();
    ImGui::Separator();
    // -------------------------

    ShowRules();
    ImGui::SameLine();
    ResetLevel(level);
    ImGui::SameLine();
    SolveLevel(level);

    // -------------------------
    ImGui::Separator();
    ImGui::Separator();
    // -------------------------

    // The following can be considered "in game" UI
    if (level.HasLoaded())
    {
        // If the level is still in progress, we show the time elapsed
        if (UiSettings::LEVEL_COMPLETED_TIME == INT_MAX)
        {
            ShowElapsedTime(level);
        }
        // Else, we show the time it took to complete it
        else
        {
            ShowLevelCompleted();
        }
    }
}

void UiManager::End()
{
    ImGui::End();
}

void UiManager::Render(sf::RenderWindow& window)
{
    ImGui::SFML::Render(window);
}

void UiManager::UpdateWindowTitle(sf::RenderWindow& window)
{
    if (ImGui::InputText("Window title", GlobalSettings::WINDOW_TITLE, 255))
    {
        window.setTitle(GlobalSettings::WINDOW_TITLE);
    }
}

void UiManager::ShowFPS(float fps)
{
    ImGui::Text("FPS: %f", fps);
}

void UiManager::UpdateMousePosition(sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (mousePos.x >= 0 && mousePos.x <= GlobalSettings::WINDOW_WIDTH && mousePos.y >= 0 &&
        mousePos.y <= GlobalSettings::WINDOW_HEIGHT)
    {
        ImGui::Text("Mouse position: (%d, %d)", mousePos.x, mousePos.y);
    }
    else
    {
        ImGui::Text("Mouse out of window");
    }
}

void UiManager::UpdateBackgroundColor()
{
    if (ImGui::ColorEdit3("Background color", GlobalSettings::BACKGROUND_COLOR_INPUT))
    {
        GlobalSettings::BACKGROUND_COLOR.r = static_cast<sf::Uint8>(GlobalSettings::BACKGROUND_COLOR_INPUT[0] * 255.f);
        GlobalSettings::BACKGROUND_COLOR.g = static_cast<sf::Uint8>(GlobalSettings::BACKGROUND_COLOR_INPUT[1] * 255.f);
        GlobalSettings::BACKGROUND_COLOR.b = static_cast<sf::Uint8>(GlobalSettings::BACKGROUND_COLOR_INPUT[2] * 255.f);
    }
}

void UiManager::ShowRules()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.1f, 0.8f, 1.0f));
    if (ImGui::Button("Show rules"))
    {
        ImGui::OpenPopup("Rules");
    }
    ImGui::PopStyleColor(2);

    if (ImGui::BeginPopupModal("Rules", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("How to play");
        ImGui::Separator();
        ImGui::Text("1. Your goal is to have exactly one Q(ueen) in each row, column and color region.");
        ImGui::Text("2. Click once to place X and click again for Q. Use X to mark where Q cannot be placed.");
        ImGui::Text("3. Two Q(ueens) cannot touch each other, not even diagonally.");
        ImGui::Separator();
        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void UiManager::DebugUi(Level& level)
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Debug");
    
    ImGui::Checkbox("Show debug options", &UiSettings::SHOW_DEBUG_OPTIONS);

    if (UiSettings::SHOW_DEBUG_OPTIONS)
    {
        // Print level to console
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.1f, 0.6f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.3f, 0.6f, 1.0f));
        if (ImGui::Button("Print level to console"))
        {
            level.PrintRepresentation();
        }
        ImGui::PopStyleColor(2);
    }
}

void UiManager::LoadLevel(Level& level)
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Load");

    // This prevents the program from crashing from a clean slate (i.e., no levels have been saved yet)
    if (m_levelsToLoad.empty())
    {
        ImGui::Text("Save at least 1 level so levels can be loaded.");
        return;
    }

    if (ImGui::BeginCombo("Level to load", m_levelsToLoad[UiSettings::LEVEL_CURRENT_INDEX].data(), 0))
    {
        for (size_t i = 0; i < m_levelsToLoad.size(); i++)
        {
            const bool isSelected = (UiSettings::LEVEL_CURRENT_INDEX == i);
            if (ImGui::Selectable(m_levelsToLoad[i].data(), isSelected))
            {
                UiSettings::LEVEL_CURRENT_INDEX = i;
                UiSettings::LEVEL_COMPLETED_TIME = INT_MAX; // Reset level completed time

                level.Clear();
                level.Load(m_levelsToLoad[UiSettings::LEVEL_CURRENT_INDEX]);
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void UiManager::ResetLevel(Level& level)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
    if (ImGui::Button("Reset"))
    {
        level.Reset();
    }
    ImGui::PopStyleColor(2);
}

void UiManager::SolveLevel(Level& level)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.4f, 0.0f, 1.0f));
    if (ImGui::Button("Solve"))
    {
        level.Solve();
    }
    ImGui::PopStyleColor(2);
}

void UiManager::ShowElapsedTime(Level& level)
{
    const int timeElapsed = static_cast<int>(level.GetClock().getElapsedTime().asSeconds());
    ImGui::Text("Time elapsed: %d seconds", timeElapsed);
}

void UiManager::ShowLevelCompleted()
{
    ImGui::OpenPopup("Level completed");

    if (ImGui::BeginPopupModal("Level completed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Good job! You completed the level in %d seconds!", UiSettings::LEVEL_COMPLETED_TIME);
        ImGui::Separator();
        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void UiManager::GetLevelsToLoad()
{
    m_levelsToLoad.clear();

    for (const auto& entry : std::filesystem::directory_iterator(std::string(GlobalSettings::LEVELS_PATH)))
    {
        const auto& path = entry.path();
        if (path.extension() == ".txt")
        {
            m_levelsToLoad.emplace_back(path.filename().string());
        }
    }

    // Sort level files alphabetically
    std::sort(
        m_levelsToLoad.begin(),
        m_levelsToLoad.end(),
        [](const std::string& a, const std::string& b) -> bool { return a < b; });
}
