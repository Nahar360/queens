#ifndef WORLD_HPP
#define WORLD_HPP

#include <string>
#include <vector>

#include "SFML/Graphics/Rect.hpp"
#include "Tile.hpp"

class CWorld
{
public:
    CWorld() = default;
    ~CWorld() = default;

    void Init();

    bool HasLoaded();

    void Clear();
    void Update(sf::RenderWindow& window);
    void MouseDetection(sf::Mouse::Button mouseButton, sf::Vector2i mousePos);

    bool IsMousePosWithinWorldBounds(sf::Vector2i mousePos) const;
    void ChangeHoveredTileColor(sf::Vector2i mousePos);

    void PrintRepresentation();

    void Load(const std::string& worldFileName);

    bool Check();
    void ClearMarks();
    void Reveal();

private:
    std::vector<std::vector<CTile>> m_tiles;
    int m_numberOfDifferentRegions = 0;

    sf::FloatRect m_globalBounds;

    // Textures
    sf::Texture m_xIconTexture;
    sf::Texture m_queenIconTexture;
    sf::Texture m_transparentIconTexture;

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
