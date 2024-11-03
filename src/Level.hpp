#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include "Tile.hpp"

using ColorInfo = std::pair<std::string, sf::Color>;

class Level
{
public:
    Level() = default;
    ~Level() = default;

    void Init(const std::string& levelFileName);

    bool HasLoaded();

    void Clear();
    void Update(sf::RenderWindow& window);
    void MouseDetection(sf::Mouse::Button mouseButton, const sf::Vector2i& mousePos);

    bool IsMousePosWithinLevelBounds(const sf::Vector2i& mousePos) const;
    void ChangeHoveredTileColor(const sf::Vector2i& mousePos);

    void PrintRepresentation();

    void Load(const std::string& levelFileName);

    bool Check();
    void Reset();
    void Solve();

    // Static variables
    static const std::vector<ColorInfo> REGIONS_COLORS;
    static const std::vector<sf::Vector2i> NEIGHBOURS_OFFSETS;

    // Getters
    sf::Clock GetClock() const { return m_clock; }

private:
    std::vector<std::vector<Tile>> m_tiles;
    std::unordered_map<int, ColorInfo> m_regionsColors;
    sf::FloatRect m_globalBounds;
    sf::Clock m_clock;
    std::vector<Tile> m_queens; // helper member variable to keep track of queens

    // Helper functions
    void InitTilesFromRepr(const std::vector<std::vector<int>>& repr);

    int GetNumberOfQueensInVector(const std::vector<Tile>& tiles) const;
    std::vector<Tile> GetEmptyTilesInVector(const std::vector<Tile>& tiles) const;

    const std::vector<Tile>& GetTilesInRow(int row) const;
    std::vector<Tile> GetTilesInColumn(int column) const;

    std::vector<Tile> GetTilesInRegion(int colorId) const;
    std::vector<Tile> GetTilesInRegion(const std::string& colorStr) const;

    // 'Check' helper functions
    void InternalCheck();
    bool CheckRows();
    bool CheckColumns();
    bool CheckRegions();
    bool CheckProximities();

    // 'Proximity' helper functions
    std::vector<Tile> GetNeighboursOfTile(const Tile& tile);
    bool IsCoordInBounds(const sf::Vector2i coord);

    // 'Cross out' helper functions
    bool CrossOutTilesInRow(const Tile& tile);
    bool CrossOutTilesInColumn(const Tile& tile);
    bool CrossOutTilesInProximity(const Tile& tile);
    bool CrossOutTilesInRegion(const Tile& tile);

    // 'Color' helper functions
    const std::string& ColorIdToColorStr(int colorId) const;
    int ColorStrToColorId(const std::string& colorStr) const;

    // 'Solve' helper functions
    /* 1 */ bool QueensCrossOutRelatedTiles();
    /* 2 */ bool MarkQueenInRegionsWithOnlyOneEmptyTile();
    /* 3 */ bool MarkQueenInRowsOrColumnsWithOnlyOneEmptyTile();
    /* 4 */ bool CrossingOutTilesInRegionExceptRowOrColumn();
    /* 5 */ bool CrossOutRowOrColumnExceptRegion();
};

#endif // LEVEL_HPP
