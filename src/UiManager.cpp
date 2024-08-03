#include "UiManager.hpp"

#include <algorithm> // for std::sort
#include <filesystem>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "GlobalSettings.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "UiSettings.hpp"


void CUiManager::Init(sf::RenderWindow& window)
{
    ImGui::SFML::Init(window);

    GetWorldsToLoad();
}

void CUiManager::Shutdown()
{
    ImGui::SFML::Shutdown();
}

void CUiManager::ProcessEvent(sf::Event event)
{
    ImGui::SFML::ProcessEvent(event);
}

void CUiManager::Update(sf::RenderWindow& window)
{
    ImGui::SFML::Update(window, m_deltaClock.restart());
}

void CUiManager::Begin()
{
    ImGui::Begin("Menu");
}

void CUiManager::Run(sf::RenderWindow& window, CWorld& world, float fps)
{
    Update(window);

    Begin();

    HandleUi(window, world, fps);

    End();
}

void CUiManager::HandleUi(sf::RenderWindow& window, CWorld& world, float fps)
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

    ShowRules();

    // -------------------------
    ImGui::Separator();
    ImGui::Separator();
    // -------------------------

    LoadWorld(world);

    PrintWorldRepresentation(world);

    // -------------------------
    ImGui::Separator();
    ImGui::Separator();
    // -------------------------

    Check(world);
    ImGui::SameLine();
    ClearMarks(world);
    ImGui::SameLine();
    Reveal(world);
}

void CUiManager::End()
{
    ImGui::End();
}

void CUiManager::Render(sf::RenderWindow& window)
{
    ImGui::SFML::Render(window);
}

void CUiManager::UpdateWindowTitle(sf::RenderWindow& window)
{
    if (ImGui::InputText("Window title", GlobalSettings::WINDOW_TITLE, 255))
    {
        window.setTitle(GlobalSettings::WINDOW_TITLE);
    }
}

void CUiManager::ShowFPS(float fps)
{
    ImGui::Text("FPS: %f", fps);
}

void CUiManager::UpdateMousePosition(sf::RenderWindow& window)
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

void CUiManager::UpdateBackgroundColor()
{
    if (ImGui::ColorEdit3("Background color", GlobalSettings::BACKGROUND_COLOR_INPUT))
    {
        GlobalSettings::BACKGROUND_COLOR.r = static_cast<sf::Uint8>(GlobalSettings::BACKGROUND_COLOR_INPUT[0] * 255.f);
        GlobalSettings::BACKGROUND_COLOR.g = static_cast<sf::Uint8>(GlobalSettings::BACKGROUND_COLOR_INPUT[1] * 255.f);
        GlobalSettings::BACKGROUND_COLOR.b = static_cast<sf::Uint8>(GlobalSettings::BACKGROUND_COLOR_INPUT[2] * 255.f);
    }
}

void CUiManager::ShowRules()
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

void CUiManager::PrintWorldRepresentation(CWorld& world)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.1f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.3f, 0.6f, 1.0f));
    if (ImGui::Button("Print world to console"))
    {
        world.PrintRepresentation();
    }
    ImGui::PopStyleColor(2);
}

void CUiManager::LoadWorld(CWorld& world)
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Load");

    // This prevents the program from crashing from a clean slate (i.e., no worlds have been saved yet)
    if (m_worldsToLoad.empty())
    {
        ImGui::Text("Save at least 1 world so worlds can be loaded.");
        return;
    }

    if (ImGui::BeginCombo("World to load", m_worldsToLoad[UiSettings::WORLD_CURRENT_INDEX].data(), 0))
    {
        for (int n = 0; n < m_worldsToLoad.size(); n++)
        {
            const bool is_selected = (UiSettings::WORLD_CURRENT_INDEX == n);
            if (ImGui::Selectable(m_worldsToLoad[n].data(), is_selected))
            {
                UiSettings::WORLD_CURRENT_INDEX = n;
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.3f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.5f, 0.0f, 1.0f));
    if (ImGui::Button("Load world (from .txt file)"))
    {
        world.Clear();
        world.Load(m_worldsToLoad[UiSettings::WORLD_CURRENT_INDEX]);
    }
    ImGui::PopStyleColor(2);
}

void CUiManager::Check(CWorld& world)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.6f, 0.3f, 1.0f));
    if (ImGui::Button("Check"))
    {
        world.Check();
    }
    ImGui::PopStyleColor(2);
}

void CUiManager::ClearMarks(CWorld& world)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
    if (ImGui::Button("Clear"))
    {
        world.ClearMarks();
    }
    ImGui::PopStyleColor(2);
}

void CUiManager::Reveal(CWorld& world)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.9f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.7f, 0.0f, 1.0f));
    if (ImGui::Button("Reveal"))
    {
        world.Reveal();
    }
    ImGui::PopStyleColor(2);
}

void CUiManager::GetWorldsToLoad()
{
    m_worldsToLoad.clear();

    for (const auto& entry : std::filesystem::directory_iterator(std::string(GlobalSettings::WORLDS_PATH)))
    {
        const auto& path = entry.path();
        if (path.extension() == ".txt")
        {
            m_worldsToLoad.emplace_back(path.filename().string());
        }
    }

    // Sort world files alphabetically
    std::sort(
        m_worldsToLoad.begin(),
        m_worldsToLoad.end(),
        [](const std::string& a, const std::string& b) -> bool { return a < b; });
}
