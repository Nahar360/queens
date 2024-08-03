#include "World.hpp"

#include <climits> // for INT_MAX
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <utility>
#include <vector>

#include "GlobalSettings.hpp"
#include "SFML/Graphics/Color.hpp"
#include "Tile.hpp"

void CWorld::Init()
{
    
}

void CWorld::InitTilesFromRepr(const std::vector<std::vector<int>>& repr)
{
    Clear();

    // Initialise tiles depending on representation
    std::vector<std::pair<int, sf::Color>> colorSet;
    for (int i = 0; i < repr.size(); i++)
    {
        std::vector<CTile> tiles_row;
        const int numColumns = repr[i].size();
        for (int j = 0; j < numColumns; j++)
        {
            const int id = (i * numColumns) + j;

            const int colorId = repr[i][j];
            sf::Color color;
            bool found = false;
            for (const auto& elem : colorSet)
            {
                if (colorId == elem.first)
                {
                    color = elem.second;
                    found = true;
                }
            }
            if (!found)
            {
                const int r = rand() % 255;
                const int b = rand() % 255;
                const int g = rand() % 255;
                color = sf::Color(r, g, b);
            }
            colorSet.push_back(std::make_pair(colorId, color));
            
            const sf::Vector2i coords(i, j);
            const sf::Vector2f pos((j + 1) * GlobalSettings::TILE_SIZE, (i + 1) * GlobalSettings::TILE_SIZE);

            CTile tile(id, colorId, color, coords, pos);
            tile.Init();

            tiles_row.push_back(tile);
        }

        m_tiles.push_back(tiles_row);
    }
}

void CWorld::Clear()
{
    m_tiles.clear();
}

void CWorld::Update(sf::RenderWindow& window)
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            m_tiles[i][j].Draw(window);
        }
    }
}

void CWorld::MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos)
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            if (m_tiles[i][j].MouseDetection(mouseButton, mousePos))
            {
                break;
            }
        }
    }
}

void CWorld::PrintRepresentation()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            std::cout << static_cast<int>(m_tiles[i][j].GetColorId());
        }
        std::cout << std::endl;
    }
}

void CWorld::Load(const std::string& worldFileName)
{
    std::vector<std::vector<int>> repr;

    std::string worldFilePath = std::string(GlobalSettings::WORLDS_PATH) + worldFileName;
    std::ifstream worldFile(worldFilePath);
    if (worldFile.is_open())
    {
        std::string line;
        while (getline(worldFile, line))
        {
            std::vector<int> row;
            for (int i = 0; i < line.length(); i++)
            {
                const int tileNumber = static_cast<int>(line[i]) - 48; // 48 is the ASCII value of 0
                row.push_back(tileNumber);
            }

            repr.push_back(row);
        }

        worldFile.close();
    }

    InitTilesFromRepr(repr);
}

void CWorld::Check()
{
    // Check for rules
    // 1 and only 1 Q in each row, column and colour region
    // 2 Qs cannot touch each other, not even diagonally

    // Row check

    // Column check

    // Colour region check

    // Proximity check
}

void CWorld::ClearMarks()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            m_tiles[i][j].ClearMark();
        }
    }
}

void CWorld::Reveal()
{
    // Fill the board with X
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            m_tiles[i][j].PlaceX();
        }
    }

    // Place Q in their right tiles
    m_tiles[0][5].PlaceQueen();
    m_tiles[1][7].PlaceQueen();
    m_tiles[2][4].PlaceQueen();
    m_tiles[3][1].PlaceQueen();
    m_tiles[4][3].PlaceQueen();
    m_tiles[5][6].PlaceQueen();
    m_tiles[6][2].PlaceQueen();
    m_tiles[7][0].PlaceQueen();
}
