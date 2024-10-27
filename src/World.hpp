#ifndef WORLD_HPP
#define WORLD_HPP

#include <string>
#include <vector>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"

#include "Tile.hpp"

class CWorld
{
public:
    CWorld() = default;
    ~CWorld() = default;

    void Init(const std::string& worldFileName);

    bool HasLoaded();

    void Clear();
    void Update(sf::RenderWindow& window);
    void MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos);

    bool IsMousePosWithinWorldBounds(sf::Vector2i mousePos) const;
    void ChangeHoveredTileColor(sf::Vector2i mousePos);

    void PrintRepresentation();

    void Load(const std::string& worldFileName);

    bool Check();
    void Reset();

    // Static variables
    static const std::vector<sf::Color> REGIONS_COLORS;

    // Getters
    sf::Clock GetClock() const { return m_clock; }

private:
    std::vector<std::vector<CTile>> m_tiles;
    int m_numRegions = 0; // Number of different regions

    sf::FloatRect m_globalBounds;

    sf::Clock m_clock;

    void InitTilesFromRepr(const std::vector<std::vector<int>>& repr);

    // 'Check' helper functions
    bool CheckRows();
    bool CheckColumns();
    bool CheckRegions();
    bool CheckProximities();

    // 'Proximity' helper functions
    std::vector<bool> GetNeighboursOfTile(const CTile& tile);
    bool GetTileHasQueenWithOffset(const CTile& tile, const sf::Vector2i& offset);
    bool IsCoordInBounds(const sf::Vector2i coord);

    int GetNumberOfQueensInVector(const std::vector<CTile>& v) const;
};

#endif // WORLD_HPP
