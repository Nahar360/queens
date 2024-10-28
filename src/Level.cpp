#include "Level.hpp"

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include "GlobalSettings.hpp"
#include "Tile.hpp"
#include "UiSettings.hpp"

// clang-format off

// Define and initialize the REGIONS_COLORS static member variable (https://loading.io/color/feature/Set3-10/)
const std::vector<sf::Color> Level::REGIONS_COLORS = {
    sf::Color(141, 211, 199), // Turquoise
    sf::Color(255, 255, 179), // Light yellow
    sf::Color(190, 186, 218), // Lavender
    sf::Color(251, 128, 114), // Salmon
    sf::Color(128, 177, 211), // Sky blue
    sf::Color(253, 180, 98),  // Light orange
    sf::Color(179, 222, 105), // Light green
    sf::Color(252, 205, 229), // Light pink
    sf::Color(217, 217, 217), // Light grey
    sf::Color(188, 128, 189)  // Purple
};

// Define and initialize the NEIGHBOURS_OFFSETS static member variable
const std::vector<sf::Vector2i> Level::NEIGHBOURS_OFFSETS = {
    {-1, -1}, // top left
    {-1,  0}, // top center
    {-1,  1}, // top right
    { 0, -1}, // left
    { 0,  1}, // right
    { 1, -1}, // bottom left
    { 1,  0}, // bottom center
    { 1,  1}  // bottom right
};

// clang-format on

void Level::Init(const std::string& levelFileName)
{
    // Load a level (the first one in the already populated dropdown list) by default
    Load(levelFileName);
}

void Level::InitTilesFromRepr(const std::vector<std::vector<int>>& repr)
{
    Clear();

    if (repr.size() > 0)
    {
        UiSettings::LEVEL_COLS = repr.size();
        UiSettings::LEVEL_ROWS = repr[0].size();
    }

    // Initialise tiles depending on representation
    std::unordered_map<int, sf::Color> colorSet;
    for (int i = 0; i < repr.size(); i++)
    {
        std::vector<Tile> tiles_row;
        const int numColumns = repr[i].size();
        for (int j = 0; j < numColumns; j++)
        {
            // id
            const int id = (i * numColumns) + j;

            // colorId
            const int colorId = repr[i][j];
            if (colorId > Level::REGIONS_COLORS.size())
            {
                std::cerr << "Levels with more than " << Level::REGIONS_COLORS.size() << " colors are not supported." << std::endl;
                return;
            }
            const sf::Color color = Level::REGIONS_COLORS[colorId % Level::REGIONS_COLORS.size()];
            colorSet[colorId] = color; // add color to the set, associated to its colorId
            
            // coords
            const sf::Vector2i coords(i, j);
            
            // pos
            const sf::Vector2f pos((j + 1) * GlobalSettings::TILE_SIZE, (i + 1) * GlobalSettings::TILE_SIZE);

            Tile tile(id, colorId, color, coords, pos);
            tile.Init();

            tiles_row.emplace_back(tile);
        }

        m_tiles.emplace_back(tiles_row);
    }

    // The number of regions is equal to the number of different colors
    m_numRegions = colorSet.size();
}

bool Level::HasLoaded()
{
    return m_tiles.size() > 0;
}

void Level::Clear()
{
    UiSettings::LEVEL_COMPLETED_TIME = INT_MAX;

    m_tiles.clear();
    m_numRegions = 0;
    m_clock.restart();
}

void Level::Update(sf::RenderWindow& window)
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            m_tiles[i][j].Draw(window);
        }
    }
}

void Level::MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos)
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            if (m_tiles[i][j].MouseDetection(mouseButton, mousePos))
            {
                // Check if the level has been completed after each move
                const bool levelCompleted = Check();
                if (levelCompleted)
                {
                    // If the level has been completed, we save the time it took to complete it
                    UiSettings::LEVEL_COMPLETED_TIME = static_cast<int>(m_clock.getElapsedTime().asSeconds());
                }

                break;
            }
        }
    }
}

bool Level::IsMousePosWithinLevelBounds(sf::Vector2i mousePos) const
{
    return m_globalBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

void Level::ChangeHoveredTileColor(sf::Vector2i mousePos)
{
    if (IsMousePosWithinLevelBounds(mousePos))
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
    // For the case that we are hovering over a tile but we go out of level bounds
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

void Level::PrintRepresentation()
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

void Level::Load(const std::string& levelFileName)
{
    std::vector<std::vector<int>> repr;

    std::string levelFilePath = std::string(GlobalSettings::LEVELS_PATH) + levelFileName;
    std::ifstream levelFile(levelFilePath);
    if (levelFile.is_open())
    {
        std::string line;
        while (getline(levelFile, line))
        {
            std::vector<int> row;
            for (int i = 0; i < line.length(); i++)
            {
                const int tileNumber = static_cast<int>(line[i]) - 48; // 48 is the ASCII value of 0
                row.emplace_back(tileNumber);
            }

            repr.emplace_back(row);
        }

        levelFile.close();
    }

    InitTilesFromRepr(repr);

    // Save level's coordinates
    if (HasLoaded())
    {
        const Tile firstTile = m_tiles[0][0];
        const sf::FloatRect firstTileGlobalBounds = firstTile.GetGlobalBounds();
        const sf::Vector2f topLeftCoord = {firstTileGlobalBounds.top, firstTileGlobalBounds.left};
        const sf::Vector2f sizeRect = sf::Vector2f(GlobalSettings::TILE_SIZE * m_tiles.size(), GlobalSettings::TILE_SIZE * m_tiles.size());
        m_globalBounds = sf::FloatRect(topLeftCoord, sizeRect);
    }
}

bool Level::Check()
{
    // 1 and only 1 Q in each row, column and colour region
    const bool rowsCheckSuccessful = CheckRows();
    const bool columnsCheckSuccessful = CheckColumns();
    const bool regionsCheckSuccessful = CheckRegions();

    // 2 Qs cannot touch each other, not even diagonally
    const bool proximitiesCheckSuccessful = CheckProximities();

    return rowsCheckSuccessful && columnsCheckSuccessful && regionsCheckSuccessful && proximitiesCheckSuccessful;
}

bool Level::CheckRows()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        const int numberOfQueensInRow = GetNumberOfQueensInVector(m_tiles[i]);
        const int rowNumber = i + 1;
        if (numberOfQueensInRow == 0)
        {
            std::cout << "No queen in row " << rowNumber << std::endl;

            return false;
        }
        else if (numberOfQueensInRow > 1)
        {
            std::cout << "More than 1 queen in row " << rowNumber << std::endl;

            return false;
        }
    }

    return true;
}

bool Level::CheckColumns()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        std::vector<Tile> column;
        // Allocate the required memory for the vector in one go, so it avoids multiple allocations (which would involve copying the elements to a new memory location)
        column.reserve(UiSettings::LEVEL_ROWS);
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            column.emplace_back(m_tiles[j][i]);
        }

        const int numberOfQueensInColumn = GetNumberOfQueensInVector(column);
        const int columnNumber = i + 1;
        if (numberOfQueensInColumn == 0)
        {
            std::cout << "No queen in column " << columnNumber << std::endl;

            return false;
        }
        else if (numberOfQueensInColumn > 1)
        {
            std::cout << "More than 1 queen in column " << columnNumber << std::endl;

            return false;
        }
    }

    return true;
}

bool Level::CheckRegions()
{
    for (int i = 0; i < m_numRegions; i++)
    {
        const std::vector<Tile>& region = GetRegionTiles(i);

        const int numberOfQueensInRegion = GetNumberOfQueensInVector(region);
        const int regionNumber = i + 1;
        if (numberOfQueensInRegion == 0)
        {
            std::cout << "No queen in region " << regionNumber << std::endl; // TODO: Add region colour

            return false;
        }
        else if (numberOfQueensInRegion > 1)
        {
            std::cout << "More than 1 queen in region " << regionNumber << std::endl; // TODO: Add region colour

            return false;
        }
    }

    return true;
}

bool Level::CheckProximities()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            if (m_tiles[i][j].isMarkQueen())
            {
                const std::vector<Tile>& neighbours = GetNeighboursOfTile(m_tiles[i][j]);
                int numberOfQueensInProximity = 0;
                for (int k = 0; k < neighbours.size(); k++)
                {
                    if (neighbours[k].isMarkQueen())
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

std::vector<Tile> Level::GetNeighboursOfTile(const Tile& tile)
{
    std::vector<Tile> neighbours;

    const sf::Vector2i& tileCoords = tile.GetCoords();
    for (const sf::Vector2i& offset : Level::NEIGHBOURS_OFFSETS)
    {
        const sf::Vector2i& otherTileCoord = sf::Vector2i(tileCoords.x + offset.x, tileCoords.y + offset.y);
        if (IsCoordInBounds(otherTileCoord))
        {
            const Tile& otherTile = m_tiles[otherTileCoord.x][otherTileCoord.y];
            neighbours.emplace_back(otherTile);
        }
    }

    return neighbours;
}

bool Level::IsCoordInBounds(const sf::Vector2i coord)
{
    // clang-format off
    const bool xCoordIsNotNegative          = coord.x >= 0;
    const bool yCoordIsNotNegative          = coord.y >= 0;
    const bool xCoordIsNotBiggerThanNumCols = coord.x < UiSettings::LEVEL_COLS;
    const bool yCoordIsNotBiggerThanNumRows = coord.y < UiSettings::LEVEL_ROWS;
    // clang-format on

    const bool isCoordInBounds =
        xCoordIsNotNegative && yCoordIsNotNegative && xCoordIsNotBiggerThanNumCols && yCoordIsNotBiggerThanNumRows;

    return isCoordInBounds;
}

int Level::GetNumberOfQueensInVector(const std::vector<Tile>& tiles) const
{
    int numberOfQueens = 0;

    for (int i = 0; i < tiles.size(); i++)
    {
        if (tiles[i].isMarkQueen())
        {
            numberOfQueens++;
        }
    }

    return numberOfQueens;
}

std::vector<Tile> Level::GetEmptyTilesInVector(const std::vector<Tile>& tiles) const
{
    std::vector<Tile> emptyTiles;

    for (int i = 0; i < tiles.size(); i++)
    {
        if (tiles[i].isMarkEmpty())
        {
            emptyTiles.emplace_back(tiles[i]);
        }
    }

    return emptyTiles;
}

void Level::Reset()
{
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            m_tiles[i][j].ClearMark();
        }
    }
}

void Level::Solve()
{
    bool moveDone = false;

    // 1. Check that for all queens, all related crossing out has been done
    for (int i = 0; i < m_tiles.size(); i++)
    {
        for (int j = 0; j < m_tiles[0].size(); j++)
        {
            const Tile& tile = m_tiles[i][j];
            if (tile.isMarkQueen())
            {
                const sf::Vector2i& tileCoords = tile.GetCoords();
                const bool crossedOutAnyTilesInRow = CrossOutTilesInRow(tileCoords);
                const bool crossedOutAnyTilesInColumn = CrossOutTilesInColumn(tileCoords);
                const bool crossedOutAnyTilesInProximity = CrossOutTilesInProximity(tile);

                moveDone = crossedOutAnyTilesInRow || crossedOutAnyTilesInColumn || crossedOutAnyTilesInProximity;
                if (moveDone)
                {
                    return; // 1 move at a time
                }
            }
        }
    }

    // 2. Check if there are regions with only 1 empty tile
    // This includes:
    // - regions with exclusively only 1 tile
    // - regions with more than 1 tile but only 1 empty one
    for (int i = 0; i < m_numRegions; i++)
    {
        const std::vector<Tile>& regionTiles = GetRegionTiles(i);
        const std::vector<Tile>& emptyTilesInRegion = GetEmptyTilesInVector(regionTiles);

        // If there is only 1 empty tile in the region
        if (emptyTilesInRegion.size() == 1)
        {
            const Tile& tile = emptyTilesInRegion[0];
            const sf::Vector2i& tileCoords = tile.GetCoords();
            
            // First, we mark the tile as a queen
            m_tiles[tileCoords.x][tileCoords.y].PlaceQueen();

            std::cout << "Region " << i + 1 << " has only 1 tile available, therefore marked it with a queen (coords.: [" << tileCoords.x << ", " << tileCoords.y << "])" << std::endl;

            // Second, we cross out the rest of the tiles in same row, column and its proximity
            // (we would also cross out in the same region, but in this case there is only 1 tile in the region)
            const bool crossedOutAnyTilesInRow = CrossOutTilesInRow(tileCoords);
            const bool crossedOutAnyTilesInColumn = CrossOutTilesInColumn(tileCoords);
            const bool crossedOutAnyTilesInProximity = CrossOutTilesInProximity(tile);

            moveDone = crossedOutAnyTilesInRow || crossedOutAnyTilesInColumn || crossedOutAnyTilesInProximity;
            if (moveDone)
            {
                return; // 1 move at a time
            }
        }
    }

    // 3. Check if there any regions for which all its empty tiles are in the same row or column
    //    If so, cross out the rest of the tiles in the row or column which do not correspond to the region itself

    // 4. Check if there are any rows or columns which are completely of the same colour
    //    If so, cross out the rest of the tiles in the region of that colour
    //    Edge case: if there's a row and a column of the same colour which coincide in a tile, mark it as a queen and cross out the rest of the tiles in the region
}

std::vector<Tile> Level::GetRegionTiles(int regionColorId) const
{
    std::vector<Tile> region;

    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[i].size(); j++)
        {
            const Tile& tileCandidate = m_tiles[i][j];
            if (tileCandidate.GetColorId() == regionColorId)
            {
                region.emplace_back(tileCandidate);
            }
        }
    }

    return region;
}

bool Level::CrossOutTilesInRow(const sf::Vector2i& tileCoords)
{
    bool crossedOutAny = false;

    std::cout << "Crossing out tiles in row " << tileCoords.x << std::endl;

    for (size_t j = 0; j < m_tiles.size(); j++)
    {
        if (j != tileCoords.y)
        {
            Tile& tileCandidate = m_tiles[tileCoords.x][j];
            // Only if the tile has still not been marked as a Queen or X, i.e., it's empty
            if (tileCandidate.isMarkEmpty())
            {
                tileCandidate.PlaceX();

                // We set the flag to true if we have crossed out any tile
                if (!crossedOutAny)
                {
                    crossedOutAny = true;
                }
            }
        }
    }

    return crossedOutAny;
}

bool Level::CrossOutTilesInColumn(const sf::Vector2i& tileCoords)
{
    bool crossedOutAny = false;

    std::cout << "Crossing out tiles in column " << tileCoords.y << std::endl;

    for (size_t i = 0; i < m_tiles[0].size(); i++)
    {
        if (i != tileCoords.x)
        {
            Tile& tileCandidate = m_tiles[i][tileCoords.y];
            // Only if the tile has still not been marked as a Queen or X, i.e., it's empty
            if (tileCandidate.isMarkEmpty())
            {
                tileCandidate.PlaceX();

                // We set the flag to true if we have crossed out any tile
                if (!crossedOutAny)
                {
                    crossedOutAny = true;
                }
            }
        }
    }

    return crossedOutAny;
}

bool Level::CrossOutTilesInProximity(const Tile& tile)
{
    bool crossedOutAny = false;

    std::cout << "Crossing out tiles in proximity of tile " << tile.GetId() << std::endl;

    const std::vector<Tile>& neighbours = GetNeighboursOfTile(tile);
    for (const Tile& neighbour : neighbours)
    {
        if (neighbour.isMarkEmpty())
        {
            const sf::Vector2i neighbourCoords = neighbour.GetCoords();
            m_tiles[neighbourCoords.x][neighbourCoords.y].PlaceX();

            // We set the flag to true if we have crossed out any tile
            if (!crossedOutAny)
            {
                crossedOutAny = true;
            }
        }
    }

    return crossedOutAny;
}
