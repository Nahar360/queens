#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <string>
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
    void MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos);

    bool IsMousePosWithinLevelBounds(sf::Vector2i mousePos) const;
    void ChangeHoveredTileColor(sf::Vector2i mousePos);

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

    void InitTilesFromRepr(const std::vector<std::vector<int>>& repr);

    // 'Check' helper functions
    void InternalCheck();
    bool CheckRows();
    bool CheckColumns();
    bool CheckRegions();
    bool CheckProximities();

    // 'Proximity' helper functions
    std::vector<Tile> GetNeighboursOfTile(const Tile& tile);
    bool IsCoordInBounds(const sf::Vector2i coord);

    int GetNumberOfQueensInVector(const std::vector<Tile>& tiles) const;
    std::vector<Tile> GetEmptyTilesInVector(const std::vector<Tile>& tiles) const;

    std::vector<Tile> GetTilesInRegion(int colorId) const;
    std::vector<Tile> GetTilesInRegion(const std::string& colorStr) const;

    bool CrossOutTilesInRow(const Tile& tile);
    bool CrossOutTilesInColumn(const Tile& tile);
    bool CrossOutTilesInProximity(const Tile& tile);
    bool CrossOutTilesInRegion(const Tile& tile);

    // Super helper functions
    std::string ColorIdToColorStr(int colorId) const;
    int ColorStrToColorId(const std::string& colorStr) const;
};

#endif // LEVEL_HPP
