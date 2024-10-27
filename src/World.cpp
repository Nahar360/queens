#include "World.hpp"

#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include "GlobalSettings.hpp"
#include "Tile.hpp"
#include "UiSettings.hpp"

void CWorld::Init()
{
    // Load a world (the first one) by default
    Load("world_77.txt");
}

void CWorld::InitTilesFromRepr(const std::vector<std::vector<int>>& repr)
{
    Clear();

    if (repr.size() > 0)
    {
        UiSettings::WORLD_COLS = repr.size();
        UiSettings::WORLD_ROWS = repr[0].size();
    }

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
                m_numberOfDifferentRegions++;
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

bool CWorld::HasLoaded()
{
    return m_tiles.size() > 0;
}

void CWorld::Clear()
{
    m_tiles.clear();
    m_numberOfDifferentRegions = 0;
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

bool CWorld::IsMousePosWithinWorldBounds(sf::Vector2i mousePos) const
{
    return m_globalBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

void CWorld::ChangeHoveredTileColor(sf::Vector2i mousePos)
{
    if (IsMousePosWithinWorldBounds(mousePos))
    {
        for (int i = 0; i < m_tiles.size(); i++)
        {
            for (int j = 0; j < m_tiles[0].size(); j++)
            {
                if (m_tiles[i][j].MouseHover(mousePos))
                {
                    break;
                }
            }
        }
    }
    // For the case that we are hovering over a tile but we go out of world bounds
    // so the tile should reset its color
    else
    {
        for (int i = 0; i < m_tiles.size(); i++)
        {
            for (int j = 0; j < m_tiles[0].size(); j++)
            {
                if (m_tiles[i][j].WasBeingHovered())
                {
                    m_tiles[i][j].ResetColor();
                    m_tiles[i][j].ResetHoverState();

                    break;
                }
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

    // Save world's coordinates
    if (HasLoaded())
    {
        const CTile firstTile = m_tiles[0][0];
        const sf::FloatRect firstTileGlobalBounds = firstTile.GetGlobalBounds();
        const sf::Vector2f topLeftCoord = {firstTileGlobalBounds.top, firstTileGlobalBounds.left};
        const sf::Vector2f sizeRect = sf::Vector2f(GlobalSettings::TILE_SIZE * m_tiles.size(), GlobalSettings::TILE_SIZE * m_tiles.size());
        m_globalBounds = sf::FloatRect(topLeftCoord, sizeRect);
    }
}

bool CWorld::Check()
{
    // 1 and only 1 Q in each row, column and colour region
    const bool rowsCheckSuccessful = CheckRows();
    const bool columnsCheckSuccessful = CheckColumns();
    const bool regionsCheckSuccessful = CheckRegions();

    // 2 Qs cannot touch each other, not even diagonally
    const bool proximitiesCheckSuccessful = CheckProximities();

    return rowsCheckSuccessful && columnsCheckSuccessful && regionsCheckSuccessful && proximitiesCheckSuccessful;
}

bool CWorld::CheckRows()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        const int numberOfQueensInRow = GetNumberOfQueensInVector(m_tiles[i]);
        if (numberOfQueensInRow != 1)
        {
            std::cout << "Not 1 queen in row " << i + 1 << std::endl;

            return false;
        }
    }

    return true;
}

bool CWorld::CheckColumns()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        std::vector<CTile> column;
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            column.push_back(m_tiles[j][i]);
        }

        const int numberOfQueensInColumn = GetNumberOfQueensInVector(column);
        if (numberOfQueensInColumn != 1)
        {
            std::cout << "Not 1 queen in column " << i + 1 << std::endl;

            return false;
        }
    }

    return true;
}

bool CWorld::CheckRegions()
{
    for (int i = 1; i < m_numberOfDifferentRegions + 1; i++) // because regions start from 1
    {
        std::vector<CTile> region;
        for (int j = 0; j < m_tiles.size(); j++)
        {
            for (int k = 0; k < m_tiles[0].size(); k++)
            {
                if (m_tiles[j][k].GetColorId() == i)
                {
                    region.push_back(m_tiles[j][k]);
                }
            }
        }

        const int numberOfQueensInRegion = GetNumberOfQueensInVector(region);
        if (numberOfQueensInRegion != 1)
        {
            std::cout << "Not 1 queen in region " << i << std::endl;

            return false;
        }
    }

    return true;
}

bool CWorld::CheckProximities()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            if (m_tiles[i][j].isMarkQueen())
            {
                const std::vector<bool> neighbours = GetNeighboursOfTile(m_tiles[i][j]);
                int numberOfQueensInProximity = 0;
                for (int k = 0; k < neighbours.size(); k++)
                {
                    if (neighbours[k] == true)
                    {
                        numberOfQueensInProximity++;
                    }
                }
                if (numberOfQueensInProximity > 0)
                {
                    std::cout << "Other queens in proximity of Queen in tile " << m_tiles[i][j].GetId() << std::endl;

                    return false;
                }
            }
        }
    }

    return true;
}

std::vector<bool> CWorld::GetNeighboursOfTile(const CTile& tile)
{
    // clang-format off
    const bool topLeftTile =      GetTileHasQueenWithOffset(tile, {-1, -1});
    const bool topCenterTile =    GetTileHasQueenWithOffset(tile, {-1, 0});
    const bool topRightTile =     GetTileHasQueenWithOffset(tile, {-1, 1}); 
    const bool leftTile  =        GetTileHasQueenWithOffset(tile, {0, -1});
    const bool rightTile =        GetTileHasQueenWithOffset(tile, {0, 1});
    const bool bottomLeftTile =   GetTileHasQueenWithOffset(tile, {1, -1});
    const bool bottomCenterTile = GetTileHasQueenWithOffset(tile, {1, 0});
    const bool bottomRightTile =  GetTileHasQueenWithOffset(tile, {1, 1});
    // clang-format on

    std::vector<bool> neighbours{
        topLeftTile, topCenterTile, topRightTile, leftTile,
        rightTile, bottomLeftTile, bottomCenterTile, bottomRightTile
    };

    return neighbours;
}

bool CWorld::GetTileHasQueenWithOffset(const CTile& tile, const sf::Vector2i& offset)
{
    const sf::Vector2i tileCoords = tile.GetCoords();
    const sf::Vector2i otherTileCoord = sf::Vector2i(tileCoords.x + offset.x, tileCoords.y + offset.y);

    if (IsCoordInBounds(otherTileCoord))
    {
        return m_tiles[otherTileCoord.x][otherTileCoord.y].isMarkQueen();
    }

    // If it is out of bounds, we just return false
    return false;
}

bool CWorld::IsCoordInBounds(const sf::Vector2i coord)
{
    const bool xCoordIsNotNegative = coord.x >= 0;
    const bool yCoordIsNotNegative = coord.y >= 0;
    const bool xCoordIsNotBiggerThanNumCols = coord.x < UiSettings::WORLD_COLS;
    const bool yCoordIsNotBiggerThanNumRows = coord.y < UiSettings::WORLD_ROWS;

    const bool isCoordInBounds =
        xCoordIsNotNegative && yCoordIsNotNegative && xCoordIsNotBiggerThanNumCols && yCoordIsNotBiggerThanNumRows;

    return isCoordInBounds;
}

int CWorld::GetNumberOfQueensInVector(const std::vector<CTile>& v) const
{
    int numberOfQueens = 0;

    for (int i = 0; i < v.size(); i++)
    {
        if (v[i].isMarkQueen())
        {
            numberOfQueens++;
        }
    }

    return numberOfQueens;
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
    // TODO: Implement
}
