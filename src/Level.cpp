#include "Level.hpp"

#include <algorithm> // std::all_of
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "GlobalSettings.hpp"
#include "Tile.hpp"
#include "UiSettings.hpp"

// clang-format off

// Define and initialize the REGIONS_COLORS static member variable (https://loading.io/color/feature/Set3-10/)
const std::vector<ColorInfo> Level::REGIONS_COLORS = {
    {"Turquoise",    sf::Color(141, 211, 199)},
    {"Light yellow", sf::Color(255, 255, 179)},
    {"Lavender",     sf::Color(190, 186, 218)},
    {"Salmon",       sf::Color(251, 128, 114)},
    {"Sky blue",     sf::Color(128, 177, 211)},
    {"Light orange", sf::Color(253, 180, 98)},
    {"Light green",  sf::Color(179, 222, 105)},
    {"Light pink",   sf::Color(252, 205, 229)},
    {"Light grey",   sf::Color(217, 217, 217)},
    {"Purple",       sf::Color(188, 128, 189)}
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
    for (size_t i = 0; i < repr.size(); i++)
    {
        std::vector<Tile> tiles_row;
        const int numColumns = repr[i].size();
        for (size_t j = 0; j < numColumns; j++)
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
            const ColorInfo color = Level::REGIONS_COLORS[colorId % Level::REGIONS_COLORS.size()];
            m_regionsColors[colorId] = color; // add color to the set, associated to its colorId
            
            // coords
            const sf::Vector2i coords(i, j);
            
            // pos
            const sf::Vector2f pos((j + 1) * GlobalSettings::TILE_SIZE, (i + 1) * GlobalSettings::TILE_SIZE);

            Tile tile(id, colorId, color.second, coords, pos);
            tile.Init();

            tiles_row.emplace_back(tile);
        }

        m_tiles.emplace_back(tiles_row);
    }
}

bool Level::HasLoaded()
{
    return m_tiles.size() > 0;
}

void Level::Clear()
{
    Reset();

    m_tiles.clear();
    m_regionsColors.clear();
}

void Level::Update(sf::RenderWindow& window)
{
    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[0].size(); j++)
        {
            m_tiles[i][j].Draw(window);
        }
    }
}

void Level::InternalCheck()
{
    // Check if the level has been completed after each move
    UiSettings::LEVEL_COMPLETED = Check();
    if (UiSettings::LEVEL_COMPLETED && !UiSettings::POPUP_HAS_BEEN_CLOSED)
    {
        // If the level has been completed, we save the time it took to complete it
        UiSettings::LEVEL_COMPLETED_TIME = static_cast<int>(m_clock.getElapsedTime().asSeconds());
    }
}

void Level::MouseDetection(sf::Mouse::Button mouseButton, const sf::Vector2i& mousePos)
{
    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[0].size(); j++)
        {
            if (m_tiles[i][j].MouseDetection(mouseButton, mousePos))
            {
                InternalCheck();

                break;
            }
        }
    }
}

bool Level::IsMousePosWithinLevelBounds(const sf::Vector2i& mousePos) const
{
    return m_globalBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

void Level::ChangeHoveredTileColor(const sf::Vector2i& mousePos)
{
    if (IsMousePosWithinLevelBounds(mousePos))
    {
        for (size_t i = 0; i < m_tiles.size(); i++)
        {
            for (size_t j = 0; j < m_tiles[0].size(); j++)
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
        for (size_t i = 0; i < m_tiles.size(); i++)
        {
            for (size_t j = 0; j < m_tiles[0].size(); j++)
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
    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[0].size(); j++)
        {
            std::cout << m_tiles[i][j].GetColorId();
        }
        std::cout << std::endl;
    }
}

void Level::Load(const std::string& levelFileName)
{
    std::vector<std::vector<int>> repr;

    const std::string levelFilePath = std::string(GlobalSettings::LEVELS_PATH) + levelFileName;
    std::ifstream levelFile(levelFilePath);
    if (levelFile.is_open())
    {
        std::string line;
        while (getline(levelFile, line))
        {
            std::vector<int> row;
            for (size_t i = 0; i < line.length(); i++)
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
    std::cout << "----- Checking start..." << std::endl;
    // 1 and only 1 Q in each row, column and colour region
    const bool rowsCheckSuccessful = CheckRows();
    const bool columnsCheckSuccessful = CheckColumns();
    const bool regionsCheckSuccessful = CheckRegions();

    // 2 Qs cannot touch each other, not even diagonally
    const bool proximitiesCheckSuccessful = CheckProximities();

    std::cout << "----- Checking end..." << std::endl;

    return rowsCheckSuccessful && columnsCheckSuccessful && regionsCheckSuccessful && proximitiesCheckSuccessful;
}

bool Level::CheckRows()
{
    for (size_t i = 0; i < m_tiles.size(); i++)
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
    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        std::vector<Tile> column;
        // Allocate the required memory for the vector in one go, so it avoids multiple allocations (which would involve copying the elements to a new memory location)
        column.reserve(UiSettings::LEVEL_ROWS);
        for (size_t j = 0; j < m_tiles[0].size(); j++)
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
    const size_t numRegions = m_regionsColors.size();
    for (size_t i = 0; i < numRegions; i++)
    {
        const std::vector<Tile>& regionTiles = GetTilesInRegion(m_regionsColors.at(i).first);
        const int numberOfQueensInRegion = GetNumberOfQueensInVector(regionTiles);
        const std::string colorStr = ColorIdToColorStr(i);

        if (numberOfQueensInRegion == 0)
        {
            std::cout << "No queen in region " << colorStr << std::endl;

            return false;
        }
        else if (numberOfQueensInRegion > 1)
        {
            std::cout << "More than 1 queen in region " << colorStr << std::endl;

            return false;
        }
    }

    return true;
}

bool Level::CheckProximities()
{
    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[0].size(); j++)
        {
            if (m_tiles[i][j].isMarkQueen())
            {
                const std::vector<Tile>& neighbours = GetNeighboursOfTile(m_tiles[i][j]);
                int numberOfQueensInProximity = 0;
                for (size_t k = 0; k < neighbours.size(); k++)
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

    for (size_t i = 0; i < tiles.size(); i++)
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

    for (size_t i = 0; i < tiles.size(); i++)
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
    UiSettings::LEVEL_COMPLETED = false;
    UiSettings::LEVEL_COMPLETED_TIME = INT_MAX;
    UiSettings::POPUP_HAS_BEEN_CLOSED = false;

    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[0].size(); j++)
        {
            m_tiles[i][j].ClearMark();
        }
    }

    m_queens.clear();

    m_clock.restart();
}

void Level::Solve()
{
    std::cout << "---------- Solving start..." << std::endl;

    // After each move, we will check if the level has been completed
    // also, we will only make 1 move at a time, so it is easy to track the changes
    bool moveDone = false;

    // 1 - For all queens, make sure all related crossing out has been done
    moveDone = QueensCrossOutRelatedTiles();
    if (moveDone)
    {
        InternalCheck();
        return;
    }

    // 2 - Check if there are regions with only 1 empty tile
    //     This includes:
    //     - regions with exclusively only 1 tile
    //     - regions with more than 1 tile but only 1 empty one
    //     If so, mark it as a queen
    moveDone = MarkQueenInRegionsWithOnlyOneEmptyTile();
    if (moveDone)
    {
        InternalCheck();
        return;
    }
    
    // 3 - Check if there are any rows or columns with only 1 empty tile
    //     If so, mark it as a queen
    moveDone = MarkQueenInRowsOrColumnsWithOnlyOneEmptyTile();
    if (moveDone)
    {
        InternalCheck();
        return;
    }

    // 4. Check if there are any rows or columns which are completely of the same colour
    //    If so, cross out the rest of the tiles in the region of that colour
    //    Edge case: if there's a row and a column of the same colour which coincide in a tile, mark it as a queen and cross out the rest of the tiles in the region
    moveDone = CrossingOutTilesInRegionExceptRowOrColumn();
    if (moveDone)
    {
        InternalCheck();
        return;
    }

    // 5. Check if there any regions for which all its empty tiles are in the same row or column
    //    If so, cross out the rest of the tiles in the row or column which do not correspond to the region itself
    moveDone = CrossOutRowOrColumnExceptRegion();
    if (moveDone)
    {
        InternalCheck();
        return;
    }

    // TODO

    // 6 - Check if there are at least 2 regions that uniquely share some rows or columns,
    //     if so, cross out the rest of the tiles in those rows or columns which do not correspond to the regions themselves
    //     because we know for sure queens in those rows and columns will have to be placed in those regions

    // 7 - Check if there any tiles that if marked as a queen, would make for a row, column or region to be completely crossed out
    //     if so, that tile should be crossed out because a queen cannot be placed there

    std::cout << "---------- Solving end..." << std::endl;
}

bool Level::QueensCrossOutRelatedTiles()
{
    bool crossedOutAnything = false;
    
    for (const Tile& queen : m_queens)
    {
        // clang-format off
        const bool crossedOutAnyTilesInRow       = CrossOutTilesInRow(queen);
        const bool crossedOutAnyTilesInColumn    = CrossOutTilesInColumn(queen);
        const bool crossedOutAnyTilesInProximity = CrossOutTilesInProximity(queen);
        const bool crossedOutAnyTilesInRegion    = CrossOutTilesInRegion(queen);
        // clang-format on

        crossedOutAnything = crossedOutAnyTilesInRow || crossedOutAnyTilesInColumn || crossedOutAnyTilesInProximity || crossedOutAnyTilesInRegion;
    }

    return crossedOutAnything;
}

bool Level::CrossOutTilesInRow(const Tile& tile)
{
    bool crossedOutAny = false;

    const sf::Vector2i& tileCoords = tile.GetCoords();
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
                    // And log a message, but only once
                    std::cout << "Crossing out tiles in row " << tileCoords.x << std::endl;

                    crossedOutAny = true;
                }
            }
        }
    }

    return crossedOutAny;
}

bool Level::CrossOutTilesInColumn(const Tile& tile)
{
    bool crossedOutAny = false;

    const sf::Vector2i& tileCoords = tile.GetCoords();
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
                    // And log a message, but only once
                    std::cout << "Crossing out tiles in column " << tileCoords.y << std::endl;
                    
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
                // And log a message, but only once
                std::cout << "Crossing out tiles in proximity of tile " << tile.GetId() << std::endl;

                crossedOutAny = true;
            }
        }
    }

    return crossedOutAny;
}

bool Level::CrossOutTilesInRegion(const Tile& tile)
{
    bool crossedOutAny = false;

    const int tileColorId = tile.GetColorId();
    const std::vector<Tile>& regionTiles = GetTilesInRegion(tileColorId);
    for (const Tile& regionTile : regionTiles)
    {
        if (regionTile.isMarkEmpty())
        {
            const sf::Vector2i regionTileCoords = regionTile.GetCoords();
            m_tiles[regionTileCoords.x][regionTileCoords.y].PlaceX();

            // We set the flag to true if we have crossed out any tile
            if (!crossedOutAny)
            {
                // And log a message, but only once
                std::cout << "Crossing out tiles in region " << tileColorId << std::endl;

                crossedOutAny = true;
            }
        }
    }

    return crossedOutAny;
}

bool Level::MarkQueenInRegionsWithOnlyOneEmptyTile()
{
    const size_t numRegions = m_regionsColors.size();
    for (size_t i = 0; i < numRegions; i++)
    {
        const std::vector<Tile>& regionTiles = GetTilesInRegion(m_regionsColors.at(i).first);
        const std::vector<Tile>& emptyTilesInRegion = GetEmptyTilesInVector(regionTiles);

        // If there is only 1 empty tile in the region
        if (emptyTilesInRegion.size() == 1)
        {
            const Tile& tile = emptyTilesInRegion[0];
            const sf::Vector2i& tileCoords = tile.GetCoords();
            
            // We mark the tile as a queen
            m_tiles[tileCoords.x][tileCoords.y].PlaceQueen();
            m_queens.emplace_back(m_tiles[tileCoords.x][tileCoords.y]);

            std::cout << "Region " << i + 1 << " has only 1 tile available, therefore marked it with a queen (coords.: [" << tileCoords.x << ", " << tileCoords.y << "])" << std::endl;
            
            return true;
        }
    }

    return false;
}

bool Level::MarkQueenInRowsOrColumnsWithOnlyOneEmptyTile()
{
    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        // Get the tiles in the row and column
        const std::vector<Tile>& tilesInRow = GetTilesInRow(i);
        const std::vector<Tile>& tilesInColumn = GetTilesInColumn(i);

        // Get the empty tiles in the row and column
        const std::vector<Tile>& emptyTilesInRow = GetEmptyTilesInVector(tilesInRow);
        const std::vector<Tile>& emptyTilesInColumn = GetEmptyTilesInVector(tilesInColumn);

        // Select a tile from the row or column if any of them has only 1 empty tile

        // This declares a pointer to a const Tile
        // The pointer itself is not const, so it is posible to change what it points to
        const Tile* tile = nullptr;
        if (emptyTilesInRow.size() == 1)
        {
            // Here, we are changing the pointer to point to the first element of emptyTilesInRow. This is allowed because the pointer itself is not const
            tile = &emptyTilesInRow[0];
        }
        else if (emptyTilesInColumn.size() == 1)
        {
            // Similarly, we are changing the pointer to point to the first element of emptyTilesInColumn
            tile = &emptyTilesInColumn[0];
        }
        
        if (tile)
        {
            const sf::Vector2i& tileCoords = tile->GetCoords();
            
            // We mark the tile as a queen
            m_tiles[tileCoords.x][tileCoords.y].PlaceQueen();
            m_queens.emplace_back(m_tiles[tileCoords.x][tileCoords.y]);

            const std::string rowOrColumn = emptyTilesInRow.size() == 1 ? "Row" : "Column";
            std::cout << rowOrColumn << " " << i + 1 << " has only 1 tile available, therefore marked it with a queen (coords.: [" << tileCoords.x << ", " << tileCoords.y << "])" << std::endl;

            return true;
        }
    }

    return false;
}

bool Level::CrossingOutTilesInRegionExceptRowOrColumn()
{
    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[0].size(); j++)
        {
            // TODO: I'm pretty sure I'm over-checking here by going through all rows and columns...
            //       Try to come up with a better condition to optimise this

            // Get the tiles in the row and the column
            const std::vector<Tile>& tilesInRow = GetTilesInRow(i);
            const std::vector<Tile>& tilesInColumn = GetTilesInColumn(j);

            const int regionColorId = tilesInRow[0].GetColorId(); // or tilesInColumn[0].GetColorId(), it does not matter

            // Check if all tiles in the row or column belong to the same region
            const bool tilesInRowBelongToTheSameRegion = std::all_of(tilesInRow.begin(), tilesInRow.end(), [&](const Tile& tile) {
                return regionColorId == tile.GetColorId();
            });
            const bool tilesInColumnBelongToTheSameRegion = std::all_of(tilesInColumn.begin(), tilesInColumn.end(), [&](const Tile& tile) {
                return regionColorId == tile.GetColorId();
            });

            // Edge case: if there's a row and a column of the same colour which coincide in a tile, mark it as a queen
            if (tilesInRowBelongToTheSameRegion && tilesInColumnBelongToTheSameRegion)
            {
                if (m_tiles[i][j].isMarkEmpty())
                {
                    m_tiles[i][j].PlaceQueen();
                    m_queens.emplace_back(m_tiles[i][j]);
                    
                    return true;
                }
            }
            else if (tilesInRowBelongToTheSameRegion)
            {
                // Cross out all the other tiles in the region different that are not in this row and that are empty
                const std::vector<Tile>& tilesInRegion = GetTilesInRegion(regionColorId);
                for (const Tile& tile : tilesInRegion)
                {
                    if (tile.GetCoords().x != i && tile.isMarkEmpty())
                    {
                        m_tiles[tile.GetCoords().x][tile.GetCoords().y].PlaceX();
                    }
                }
                    
                return true;
            }
            else if (tilesInColumnBelongToTheSameRegion)
            {
                // Cross out all the other tiles in the region different that are not in this row and that are empty
                const std::vector<Tile>& tilesInRegion = GetTilesInRegion(regionColorId);
                for (const Tile& tile : tilesInRegion)
                {
                    if (tile.GetCoords().y != j && tile.isMarkEmpty())
                    {
                        m_tiles[tile.GetCoords().x][tile.GetCoords().y].PlaceX();
                    }
                }

                return true;
            }

            return false;
        }
    }

    return false;
}

bool Level::CrossOutRowOrColumnExceptRegion()
{
    const size_t numRegions = m_regionsColors.size();
    for (size_t i = 0; i < numRegions; i++)
    {
        const std::vector<Tile>& regionTiles = GetTilesInRegion(m_regionsColors.at(i).first);
        const std::vector<Tile>& emptyTilesInRegion = GetEmptyTilesInVector(regionTiles);

        if (!emptyTilesInRegion.empty())
        {
            const int rowCandidate = emptyTilesInRegion[0].GetCoords().x;
            const int columnCandidate = emptyTilesInRegion[0].GetCoords().y;

            // Check if all empty tiles in region are either in the same row or column
            const bool allEmptyTilesInSameRow = std::all_of(emptyTilesInRegion.begin(), emptyTilesInRegion.end(), [&](const Tile& tile) {
                return tile.GetCoords().x == rowCandidate;
            });
            const bool allEmptyTilesInSameColumn = std::all_of(emptyTilesInRegion.begin(), emptyTilesInRegion.end(), [&](const Tile& tile) {
                return tile.GetCoords().y == columnCandidate;
            });

            if (allEmptyTilesInSameRow || allEmptyTilesInSameColumn)
            {
                // We get the rest of the tiles in the same row or column that do not belong to the region
                const std::vector<Tile>& restOfTilesInSameRowOrColumn = allEmptyTilesInSameRow ? GetTilesInRow(rowCandidate) : GetTilesInColumn(columnCandidate);
                std::vector<Tile> restOfTilesInSameRowOrColumnOfDifferentRegions;
                for (const Tile& tile : restOfTilesInSameRowOrColumn)
                {
                    if (tile.GetColorId() != i)
                    {
                        restOfTilesInSameRowOrColumnOfDifferentRegions.emplace_back(tile);
                    }
                }
                const bool areRestOfTilesInSameRowOrColumnMarkedWithX = std::all_of(restOfTilesInSameRowOrColumnOfDifferentRegions.begin(), restOfTilesInSameRowOrColumnOfDifferentRegions.end(), [&](const Tile& tile) {
                    return tile.isMarkX();
                });

                // If the rest of the tiles in the row or column are not marked with an X, we cross them out
                // else, there's nothing else to do in this row
                if (!areRestOfTilesInSameRowOrColumnMarkedWithX)
                {
                    const std::string rowOrColumn = allEmptyTilesInSameRow ? "row " + std::to_string(rowCandidate) : "column " + std::to_string(columnCandidate);
                    std::cout << "Crossing out tiles in " << rowOrColumn << std::endl;
                    for (size_t j = 0; j < m_tiles[0].size(); j++)
                    {
                        const sf::Vector2i& coords = allEmptyTilesInSameRow ? sf::Vector2i(rowCandidate, j) : sf::Vector2i(j, columnCandidate);
                        const Tile& tileCandidate = m_tiles[coords.x][coords.y];

                        // If the tile does not belong to the region and it is still empty, we cross it out
                        if (tileCandidate.GetColorId() != i && tileCandidate.isMarkEmpty())
                        {
                            m_tiles[coords.x][coords.y].PlaceX();
                        }
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

const std::vector<Tile>& Level::GetTilesInRow(int row) const
{
    return m_tiles[row];
}

std::vector<Tile> Level::GetTilesInColumn(int column) const
{
    std::vector<Tile> tiles;

    for (size_t i = 0; i < m_tiles[0].size(); i++)
    {
        tiles.emplace_back(m_tiles[i][column]);
    }

    return tiles;
}

std::vector<Tile> Level::GetTilesInRegion(int colorId) const
{
    std::vector<Tile> region;

    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[i].size(); j++)
        {
            const Tile& tileCandidate = m_tiles[i][j];
            if (tileCandidate.GetColorId() == colorId)
            {
                region.emplace_back(tileCandidate);
            }
        }
    }

    return region;
}

std::vector<Tile> Level::GetTilesInRegion(const std::string& colorStr) const
{
    std::vector<Tile> region;

    for (size_t i = 0; i < m_tiles.size(); i++)
    {
        for (size_t j = 0; j < m_tiles[i].size(); j++)
        {
            const Tile& tileCandidate = m_tiles[i][j];
            const int colorId = ColorStrToColorId(colorStr);
            if (tileCandidate.GetColorId() == colorId)
            {
                region.emplace_back(tileCandidate);
            }
        }
    }

    return region;
}

const std::string& Level::ColorIdToColorStr(int colorId) const
{
    return m_regionsColors.at(colorId).first;
}

int Level::ColorStrToColorId(const std::string& colorStr) const
{
    for (const auto& color : m_regionsColors)
    {
        if (color.second.first == colorStr)
        {
            return color.first;
        }
    }

    return -1;
}
