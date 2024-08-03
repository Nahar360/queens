#ifndef WORLD_HPP
#define WORLD_HPP

#include <optional>
#include <string>
#include <vector>

#include "Tile.hpp"

class CWorld
{
public:
    CWorld() = default;
    ~CWorld() = default;

    void Init();

    void Clear();
    void Update(sf::RenderWindow& window);
    void MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos);

    void PrintRepresentation();

    void Load(const std::string& worldFileName);

    void Check();
    void ClearMarks();
    void Reveal();

private:
    std::vector<std::vector<CTile>> m_tiles;
    int m_numberOfDifferentRegions = 0;

    void InitTilesFromRepr(const std::vector<std::vector<int>>& repr);

    // 'Check' helper functions
    void CheckRows();
    void CheckColumns();
    void CheckRegions();
    void CheckProximities();

    // 'Proximity' helper functions
    std::vector<bool> GetNeighboursOfTile(const CTile& tile);
    bool GetTileHasQueenWithOffset(const CTile& tile, const sf::Vector2i& offset);
    bool IsCoordInBounds(const sf::Vector2i coord);

    int GetNumberOfQueensInVector(const std::vector<CTile>& v) const;
};

#endif // WORLD_HPP
